import time
import sys

# ÖZEL HATA SINIFI
class MatrixException(Exception):
    """Matris işlemleri için özel hata tanımlaması."""
    pass

# YARDIMCI ÇARPIM FONKSİYONU
def multiply_elements(a, b):
    return a * b

# DOSYA OKUMA VE HATA DENETİMİ
def read_matrix(file, expected_rows, expected_cols, mat_name):
    matrix = []
    rows_read = 0
    
    while rows_read < expected_rows:
        line = file.readline()
        if not line:
            raise MatrixException(f"{mat_name}: Beklenen boyuttan daha az satir var.")
        
        line = line.strip()
        if not line: # Boş satırları atla
            continue
            
        elements = line.split()
        
        # Fazladan eleman kontrolü
        if len(elements) > expected_cols:
            raise MatrixException(f"{mat_name} satir {rows_read + 1}: Fazladan eleman saptandi.")
        # Eksik eleman kontrolü
        if len(elements) < expected_cols:
            raise MatrixException(f"{mat_name} satir {rows_read + 1}: Eksik eleman saptandi.")
            
        row = []
        for item in elements:
            # Sayısal olmayan değer kontrolü
            if not (item.isdigit() or (item.startswith('-') and item[1:].isdigit())):
                raise MatrixException(f"{mat_name}: Sayisal olmayan deger saptandi: {item}")
            row.append(int(item))
        
        matrix.append(row)
        rows_read += 1
        
    return matrix

def parse_and_validate(filename):
    try:
        with open(filename, 'r') as file:
            # Matris A Boyutları
            line = file.readline()
            while line and not line.strip(): line = file.readline()
            if not line: return
            
            n, m = map(int, line.split())
            read_matrix(file, n, m, "Matris A")
            
            # Matris B Boyutları (A'dan sonraki ilk dolu satır)
            line = file.readline()
            while line and not line.strip(): line = file.readline()
            
            if not line:
                raise MatrixException("Matris B boyutlari okunamadi.")
                
            p, q = map(int, line.split())
            
            # Boyut uyumluluğu kontrolü (m != p)
            if m != p:
                raise MatrixException(f"Uyumsuz matris boyutları: {m} != {p}")
                
            read_matrix(file, p, q, "Matris B")
            print(f"Basarili: {filename} icindeki matrisler carpilabilir.")

    except FileNotFoundError:
        print(f"Hata: {filename} dosyası bulunamadı.")
    except MatrixException as e:
        print(f"Hata ({filename}): {e}")
    except Exception as e:
        print(f"Beklenmeyen Hata: {e}")

# BENCHMARK FONKSİYONLARI
def multiply_direct(A, B, C, n, m, q):
    for i in range(n):
        for j in range(q):
            C[i][j] = 0
            for k in range(m):
                C[i][j] += A[i][k] * B[k][j]

def run_benchmarks():
    sizes = [(50, 80), (120, 150), (300, 400), (900, 700)]
    runs = 5
    print(f"\n{'Language':<15} {'Implementation':<25} {'Size':<15} {'Avg. Time (ms)'}")
    print("-" * 70)

    for n, m in sizes:
        p, q = m, n + 10
        A = [[2 for _ in range(m)] for _ in range(n)]
        B = [[3 for _ in range(q)] for _ in range(p)]
        C = [[0 for _ in range(q)] for _ in range(n)]

        start_time = time.perf_counter()
        for _ in range(runs):
            multiply_direct(A, B, C, n, m, q)
        avg_time_ms = ((time.perf_counter() - start_time) / runs) * 1000
        print(f"{'Python':<15} {'List of lists':<25} {f'{n}x{m}':<15} {int(avg_time_ms)}")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Terminalden dosya ismi verilirse hata testi yap
        parse_and_validate(sys.argv[1])
    else:
        # Argüman yoksa performans testlerini çalıştır
        print("Python Benchmarking basliyor...")
        run_benchmarks()