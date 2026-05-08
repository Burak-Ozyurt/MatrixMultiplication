import time

# --- 1. İSTİSNA YÖNETİMİ (EXCEPTION HANDLING) ---
class MatrixException(Exception):
    """Matris okuma ve doğrulama işlemleri için özel hata sınıfı."""
    pass

# --- 2. DOSYA OKUMA VE DOĞRULAMA ---
def read_matrix(lines, start_idx, expected_rows, expected_cols):
    matrix = []
    current_idx = start_idx
    
    for i in range(expected_rows):
        if current_idx >= len(lines):
            raise MatrixException("Satır eksik: Beklenen boyuttan daha az satır var.")
        
        line = lines[current_idx].strip()
        current_idx += 1
        
        if not line:
            raise MatrixException("Satır eksik: Beklenen boyuttan daha az satır var.")
        
        elements = line.split()
        
        # Hata Kontrolleri
        if len(elements) > expected_cols:
            raise MatrixException("Satırda fazladan eleman: Beklenenden fazla veri girilmiş.")
        if len(elements) < expected_cols:
            raise MatrixException("Satırda eksik eleman: Beklenenden daha az veri girilmiş.")
            
        row = []
        for item in elements:
            # Sayısal olmayan karakter kontrolü (negatif sayılar için '-' işaretini de tolere eder)
            if not (item.isdigit() or (item.startswith('-') and item[1:].isdigit())):
                raise MatrixException(f"Sayısal olmayan değer saptandı: {item}")
            row.append(int(item))
        
        matrix.append(row)
        
    return matrix, current_idx

def parse_and_validate_file(filename):
    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
            
        if not lines:
            return

        # Matris A boyutlarını oku
        first_line = lines[0].split()
        if len(first_line) != 2:
            raise MatrixException("Matris A boyutları okunamadı.")
        n, m = int(first_line[0]), int(first_line[1])
        
        # Matris A elemanlarını oku
        matrix_a, current_idx = read_matrix(lines, 1, n, m)
        
        # İki matris arasındaki boş satırları atla
        while current_idx < len(lines) and not lines[current_idx].strip():
            current_idx += 1
            
        if current_idx >= len(lines):
            raise MatrixException("Matris B boyutları bulunamadı.")
            
        # Matris B boyutlarını oku
        b_dim_line = lines[current_idx].split()
        if len(b_dim_line) != 2:
            raise MatrixException("Matris B boyutları okunamadı.")
        p, q = int(b_dim_line[0]), int(b_dim_line[1])
        current_idx += 1
        
        # Boyut uyumluluğu kontrolü (m != p)
        if m != p:
            raise MatrixException(f"Uyumsuz matris boyutları: Matris A'nın sütun sayısı ({m}), Matris B'nin satır sayısına ({p}) eşit değil.")
            
        # Matris B elemanlarını oku
        matrix_b, current_idx = read_matrix(lines, current_idx, p, q)
        
        print(f"{filename} başarıyla okundu ve doğrulandı. ({n}x{m}) x ({p}x{q})")

    except FileNotFoundError:
        print(f"Dosya bulunamadı: {filename}. Lütfen dosya yolunu kontrol edin.")
    except MatrixException as e:
        print(f"Hata ({filename}): {e}")
    except Exception as e:
        print(f"Beklenmeyen hata: {e}")

# --- 3. BENCHMARK FONKSİYONLARI ---
def multiply_elements(a, b):
    # Fonksiyon çağrısı (Function Call) etkisini ölçmek için yardımcı fonksiyon
    return a * b

def multiply_direct(A, B, C, n, m, q):
    # Döngü içinde doğrudan hesaplama (Direct Computation)
    for i in range(n):
        for j in range(q):
            C[i][j] = 0
            for k in range(m):
                C[i][j] += A[i][k] * B[k][j]

def multiply_func(A, B, C, n, m, q):
    # Ayrı bir fonksiyon çağırarak hesaplama
    for i in range(n):
        for j in range(q):
            C[i][j] = 0
            for k in range(m):
                C[i][j] += multiply_elements(A[i][k], B[k][j])

# --- 4. TEST MOTORU (DRIVER) ---
def run_benchmarks():
    # Test edilecek matris boyutları
    sizes = [(50, 80), (120, 150), (300, 400), (900, 700)]
    runs = 5

    print(f"\n{'Language':<15} {'Implementation':<25} {'Size':<15} {'Avg. Time (ms)'}")
    print("-" * 70)

    for n, m in sizes:
        p = m
        q = n + 10

        # Python'da "List of lists" veri yapısıyla test verisi oluşturma
        A = [[2 for _ in range(m)] for _ in range(n)]
        B = [[3 for _ in range(q)] for _ in range(p)]
        C = [[0 for _ in range(q)] for _ in range(n)]

        # Benchmark 2.1: Doğrudan Hesaplama
        total_time = 0
        for _ in range(runs):
            start_time = time.perf_counter()
            multiply_direct(A, B, C, n, m, q)
            total_time += (time.perf_counter() - start_time)
        avg_time_ms = (total_time / runs) * 1000
        print(f"{'Python':<15} {'Direct computation':<25} {f'{n}x{m}':<15} {int(avg_time_ms)}")

        # Benchmark 2.2: Fonksiyon Çağrısı
        total_time = 0
        for _ in range(runs):
            start_time = time.perf_counter()
            multiply_func(A, B, C, n, m, q)
            total_time += (time.perf_counter() - start_time)
        avg_time_ms = (total_time / runs) * 1000
        print(f"{'Python':<15} {'Function Call':<25} {f'{n}x{m}':<15} {int(avg_time_ms)}")

if __name__ == "__main__":
    # --- A. İSTİSNA YÖNETİMİ TESTİ ---
    # Kendi .txt test dosyalarını okutmak için aşağıdaki yorumu kaldır ve dosya adını gir:
    # parse_and_validate_file("error_test_matrix.txt")

    # --- B. PERFORMANS TESTLERİ ---
    print("Benchmarking başlıyor...")
    run_benchmarks()