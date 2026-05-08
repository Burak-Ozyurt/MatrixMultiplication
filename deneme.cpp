#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

// --- 1. İSTİSNA YÖNETİMİ (EXCEPTION HANDLING) ---
class MatrixException : public runtime_error {
public:
    MatrixException(const string& msg) : runtime_error(msg) {}
};

// --- 2. YARDIMCI FONKSİYONLAR VE DOSYA OKUMA ---
// Çarpım için fonksiyon çağrısı (Benchmark 2 için)
inline int multiplyElements(int a, int b) {
    return a * b;
}

vector<vector<int>> readMatrix(ifstream& file, int expectedRows, int expectedCols) {
    vector<vector<int>> matrix(expectedRows, vector<int>(expectedCols));
    string line;
    
    for (int i = 0; i < expectedRows; ++i) {
        if (!getline(file, line)) throw MatrixException("Satır eksik: Beklenen boyuttan daha az satır var.");
        
        stringstream ss(line);
        string temp;
        int j = 0;
        
        while (ss >> temp) {
            if (j >= expectedCols) throw MatrixException("Satırda fazladan eleman: Beklenenden fazla veri girilmiş.");
            
            // Sayısal olmayan karakter kontrolü
            for (char c : temp) {
                if (!isdigit(c) && c != '-') throw MatrixException("Sayısal olmayan değer saptandı: " + temp);
            }
            matrix[i][j] = stoi(temp);
            j++;
        }
        if (j < expectedCols) throw MatrixException("Satırda eksik eleman: Beklenenden daha az veri girilmiş.");
    }
    return matrix;
}

void parseAndValidateFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Dosya acilamadi: " << filename << endl;
        return;
    }

    try {
        string line;
        int n, m, p, q;

        // Matris A Boyutları
        if (!(file >> n >> m)) return;
        vector<vector<int>> A = readMatrix(file, n, m);

        // Boş satırı atla
        getline(file, line); 
        while (getline(file, line) && line.empty()) {} 

        // Matris B Boyutları okuma (önceki satırdan veya yeni satırdan)
        stringstream ss(line);
        if (!(ss >> p >> q)) {
            if (!(file >> p >> q)) throw MatrixException("Matris B boyutlari okunamadi.");
        }

        // Boyut Uyumluluğu Kontrolü (m != p)
        if (m != p) throw MatrixException("Uyumsuz matris boyutlari: Matris A'nin sutun sayisi (m), Matris B'nin satir sayisina (p) esit degil.");

        vector<vector<int>> B = readMatrix(file, p, q);
        cout << filename << " basariyla okundu ve dogrulandi. (" << n << "x" << m << ") x (" << p << "x" << q << ")" << endl;

    } catch (const MatrixException& e) {
        cerr << "Hata (" << filename << "): " << e.what() << endl;
    }
}

// --- 3. BENCHMARK FONKSİYONLARI ---

// Benchmark 1.1: Vector kullanarak doğrudan çarpma
void multiplyVectorDirect(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n, int m, int q) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < q; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Benchmark 1.2: Dinamik Dizi (new/delete) ile çarpma
void multiplyDynamicArray(int** A, int** B, int** C, int n, int m, int q) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < q; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Benchmark 2: Vector kullanarak fonksiyon çağrısıyla çarpma
void multiplyVectorFunc(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n, int m, int q) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < q; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += multiplyElements(A[i][k], B[k][j]);
            }
        }
    }
}

// --- 4. TEST MOTORU (DRIVER) ---
void runBenchmarks() {
    // Test edilecek matris boyutları
    vector<pair<int, int>> sizes = {{50, 80}, {120, 150}, {300, 400}, {900, 700}};
    int num_runs = 5;

    cout << left << setw(15) << "Language" << setw(25) << "Implementation" << setw(15) << "Size" << "Avg. Time (ms)" << endl;
    cout << string(70, '-') << endl;

    for (auto size : sizes) {
        int n = size.first;
        int m = size.second;
        int p = m; // Çarpılabilmesi için
        int q = n + 10; // Rastgele q boyutu

        // Test Verisi Oluşturma
        vector<vector<int>> vecA(n, vector<int>(m, 2));
        vector<vector<int>> vecB(p, vector<int>(q, 3));
        vector<vector<int>> vecC(n, vector<int>(q, 0));

        // 1. Vector Direct Benchmark
        long long totalDuration = 0;
        for (int r = 0; r < num_runs; ++r) {
            auto start = high_resolution_clock::now();
            multiplyVectorDirect(vecA, vecB, vecC, n, m, q);
            auto stop = high_resolution_clock::now();
            totalDuration += duration_cast<milliseconds>(stop - start).count();
        }
        cout << left << setw(15) << "C++" << setw(25) << "Vector (Direct)" 
             << to_string(n) + "x" + to_string(m) << "\t\t" << (totalDuration / num_runs) << endl;

        // 2. Vector Function Call Benchmark
        totalDuration = 0;
        for (int r = 0; r < num_runs; ++r) {
            auto start = high_resolution_clock::now();
            multiplyVectorFunc(vecA, vecB, vecC, n, m, q);
            auto stop = high_resolution_clock::now();
            totalDuration += duration_cast<milliseconds>(stop - start).count();
        }
        cout << left << setw(15) << "C++" << setw(25) << "Vector (Func Call)" 
             << to_string(n) + "x" + to_string(m) << "\t\t" << (totalDuration / num_runs) << endl;

        // 3. Dynamic Array Benchmark
        int** dynA = new int*[n];
        for(int i=0; i<n; ++i) { dynA[i] = new int[m]; fill_n(dynA[i], m, 2); }
        
        int** dynB = new int*[p];
        for(int i=0; i<p; ++i) { dynB[i] = new int[q]; fill_n(dynB[i], q, 3); }
        
        int** dynC = new int*[n];
        for(int i=0; i<n; ++i) { dynC[i] = new int[q]; }

        totalDuration = 0;
        for (int r = 0; r < num_runs; ++r) {
            auto start = high_resolution_clock::now();
            multiplyDynamicArray(dynA, dynB, dynC, n, m, q);
            auto stop = high_resolution_clock::now();
            totalDuration += duration_cast<milliseconds>(stop - start).count();
        }
        cout << left << setw(15) << "C++" << setw(25) << "Dynamic Array" 
             << to_string(n) + "x" + to_string(m) << "\t\t" << (totalDuration / num_runs) << endl;

        // Bellek Temizliği (Memory Cleanup)
        for(int i=0; i<n; ++i) { delete[] dynA[i]; delete[] dynC[i]; }
        for(int i=0; i<p; ++i) delete[] dynB[i];
        delete[] dynA; delete[] dynB; delete[] dynC;
    }
}

int main() {
    // 1. Dosya Okuma ve İstisna Yönetimi Testi (Kendi .txt dosyalarını buraya verebilirsin)
    // parseAndValidateFile("test_matrix.txt");

    // 2. Performans Testleri (Benchmarking)
    cout << "Benchmarking basliyor...\n" << endl;
    runBenchmarks();

    return 0;
}