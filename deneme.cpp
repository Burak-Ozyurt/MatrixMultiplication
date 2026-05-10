#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

//  EXCEPTION HANDLING 
class MatrixException : public runtime_error {
public:
    MatrixException(const string& msg) : runtime_error(msg) {}
};

//  YARDIMCI FONKSİYONLAR 
inline int multiplyElements(int a, int b) {
    return a * b;
}

// Dosyadan matris okuma ve hata denetimi.
vector<vector<int>> readMatrix(ifstream& file, int expectedRows, int expectedCols, const string& matName) {
    vector<vector<int>> matrix(expectedRows, vector<int>(expectedCols));
    string line;
    
    for (int i = 0; i < expectedRows; ++i) {
        // Satır eksikliği kontrolü 
        if (!getline(file, line) || line.empty()) {
            if (line.empty() && i < expectedRows) { i--; continue; } // Boş satırları atla
            throw MatrixException(matName + ": Beklenen boyuttan daha az satir var.");
        }
        
        stringstream ss(line);
        string val;
        int j = 0;
        while (ss >> val) {
            // Fazladan eleman kontrolü 
            if (j >= expectedCols) throw MatrixException(matName + " satir " + to_string(i+1) + ": Fazladan eleman saptandi.");
            
            // Sayısal olmayan değer kontrolü 
            for (char c : val) {
                if (!isdigit(c) && c != '-') throw MatrixException(matName + ": Sayisal olmayan deger saptandi: " + val);
            }
            matrix[i][j] = stoi(val);
            j++;
        }
        // Satırda eksik eleman kontrolü 
        if (j < expectedCols) throw MatrixException(matName + " satir " + to_string(i+1) + ": Eksik eleman saptandi.");
    }
    return matrix;
}

// Dosya okuma ve m != p doğrulaması.
void parseAndValidateFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) { cerr << "Hata: Dosya acilamadi: " << filename << endl; return; }

    try {
        int n, m, p, q;
        if (!(file >> n >> m)) return;
        string dummy; getline(file, dummy); // Satır sonunu temizle
        
        vector<vector<int>> A = readMatrix(file, n, m, "Matris A");

        // Matrisler arasındaki boşlukları atla 
        while (file.peek() == '\n' || file.peek() == '\r' || file.peek() == ' ') file.ignore();
        
        if (!(file >> p >> q)) throw MatrixException("Matris B boyutlari okunamadi.");
        getline(file, dummy);
        
        // Boyut uyumluluğu kontrolü (m != p) 
        if (m != p) throw MatrixException("Uyumsuz matris boyutlari: " + to_string(m) + " != " + to_string(p));

        vector<vector<int>> B = readMatrix(file, p, q, "Matris B");
        cout << "Basarili: " << filename << " icindeki matrisler carpilabilir (" << n << "x" << m << " * " << p << "x" << q << ")." << endl;
    } catch (const MatrixException& e) {
        cerr << "Hata (" << filename << "): " << e.what() << endl;
    }
}

// BENCHMARK FONKSİYONLARI 

// C++ Vector - Doğrudan Çarpma 
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

// C++ Dinamik Dizi - Doğrudan Çarpma 
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

// TEST MOTORU
void runBenchmarks() {
    vector<pair<int, int>> sizes = {{50, 80}, {120, 150}, {300, 400}, {900, 700}}; // Ödevdeki boyutlar 
    int num_runs = 5; // 5 kez çalıştırıp ortalama alma kuralı 

    cout << left << setw(15) << "Language" << setw(25) << "Implementation" << setw(15) << "Size" << "Avg. Time (ms)" << endl;
    cout << string(75, '-') << endl;

    for (auto size : sizes) {
        int n = size.first, m = size.second, p = m, q = n + 10;

        // Vector
        vector<vector<int>> vA(n, vector<int>(m, 2)), vB(p, vector<int>(q, 3)), vC(n, vector<int>(q, 0));

        // 1. Vector Direct 
        auto start = high_resolution_clock::now();
        for(int r=0; r<num_runs; r++) multiplyVectorDirect(vA, vB, vC, n, m, q);
        auto stop = high_resolution_clock::now();
        cout << left << setw(15) << "C++" << setw(25) << "Vector (Direct)" 
             << to_string(n)+"x"+to_string(m) << "\t\t" << duration_cast<milliseconds>(stop - start).count()/num_runs << endl;

        // 2. Dynamic Array 
        int** dA = new int*[n]; for(int i=0; i<n; i++) { dA[i] = new int[m]; fill_n(dA[i], m, 2); }
        int** dB = new int*[p]; for(int i=0; i<p; i++) { dB[i] = new int[q]; fill_n(dB[i], q, 3); }
        int** dC = new int*[n]; for(int i=0; i<n; i++) dC[i] = new int[q];

        start = high_resolution_clock::now();
        for(int r=0; r<num_runs; r++) multiplyDynamicArray(dA, dB, dC, n, m, q);
        stop = high_resolution_clock::now();
        cout << left << setw(15) << "C++" << setw(25) << "Dynamic Array" 
             << to_string(n)+"x"+to_string(m) << "\t\t" << duration_cast<milliseconds>(stop - start).count()/num_runs << endl;

        // Bellek temizliği 
        for(int i=0; i<n; i++) { delete[] dA[i]; delete[] dC[i]; }
        for(int i=0; i<p; i++) delete[] dB[i];
        delete[] dA; delete[] dB; delete[] dC;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        parseAndValidateFile(argv[1]);
    } else {
        cout << "Performans Olcumleri (Benchmarking) Baslatiliyor...\n" << endl;
        runBenchmarks();
    }
    return 0;
}