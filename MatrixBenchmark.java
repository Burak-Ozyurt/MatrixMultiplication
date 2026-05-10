import java.util.*;
import java.io.*;

// EXCEPTION HANDLING
class MatrixException extends Exception {
    public MatrixException(String message) {
        super(message);
    }
}

public class MatrixBenchmark {

    public static int multiplyElements(int a, int b) {
        return a * b;
    }

    // DOSYA OKUMA VE HATA DENETİMİ
    public static int[][] readMatrix(Scanner scanner, int expectedRows, int expectedCols, String matName) throws MatrixException {
        int[][] matrix = new int[expectedRows][expectedCols];
        
        for (int i = 0; i < expectedRows; i++) {
            if (!scanner.hasNextLine()) {
                throw new MatrixException(matName + ": Beklenen boyuttan daha az satir var.");
            }
            
            String line = scanner.nextLine().trim();
            if (line.isEmpty()) { i--; continue; } // Boş satırları atla
            
            String[] tokens = line.split("\\s+");
            
            // Fazladan eleman kontrolü
            if (tokens.length > expectedCols) {
                throw new MatrixException(matName + " satir " + (i + 1) + ": Fazladan eleman saptandi.");
            }
            // Eksik eleman kontrolü
            if (tokens.length < expectedCols) {
                throw new MatrixException(matName + " satir " + (i + 1) + ": Eksik eleman saptandi.");
            }
            
            for (int j = 0; j < expectedCols; j++) {
                try {
                    matrix[i][j] = Integer.parseInt(tokens[j]);
                } catch (NumberFormatException e) {
                    // Sayısal olmayan değer kontrolü
                    throw new MatrixException(matName + ": Sayisal olmayan deger saptandi: " + tokens[j]);
                }
            }
        }
        return matrix;
    }

    public static void validateFile(String filename) {
        try (Scanner sc = new Scanner(new File(filename))) {
            if (!sc.hasNextInt()) return;
            int n = sc.nextInt();
            int m = sc.nextInt();
            sc.nextLine(); // Boyut satırını temizle
            
            readMatrix(sc, n, m, "Matris A");

            // Matris B boyutlarını bulana kadar satırları atla
            while (sc.hasNextLine() && !sc.hasNextInt()) sc.nextLine();
            
            if (!sc.hasNextInt()) throw new MatrixException("Matris B boyutlari okunamadi.");
            
            int p = sc.nextInt();
            int q = sc.nextInt();
            sc.nextLine();

            // Boyut uyumluluğu kontrolü (m != p)
            if (m != p) {
                throw new MatrixException("Uyumsuz matris boyutlari: " + m + " != " + p);
            }

            readMatrix(sc, p, q, "Matris B");
            System.out.println("Basarili: " + filename + " icindeki matrisler carpilabilir.");

        } catch (FileNotFoundException e) {
            System.err.println("Hata: Dosya bulunamadi: " + filename);
        } catch (MatrixException e) {
            System.err.println("Hata (" + filename + "): " + e.getMessage());
        } catch (Exception e) {
            System.err.println("Beklenmeyen Hata: " + e.getMessage());
        }
    }

    // BENCHMARK FONKSİYONLARI

    // Benchmark ilkel dizi (int[][]) doğrudan çarpım
    public static void multiplyArrayDirect(int[][] A, int[][] B, int[][] C, int n, int m, int q) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < q; j++) {
                C[i][j] = 0;
                for (int k = 0; k < m; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }

    // Benchmark ArrayList çarpım 
    public static void multiplyArrayList(ArrayList<ArrayList<Integer>> A, 
                                         ArrayList<ArrayList<Integer>> B, 
                                         ArrayList<ArrayList<Integer>> C, int n, int m, int q) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < q; j++) {
                int sum = 0;
                for (int k = 0; k < m; k++) {
                    sum += A.get(i).get(k) * B.get(k).get(j);
                }
                C.get(i).set(j, sum);
            }
        }
    }

    // TEST MOTORU 
    public static void runBenchmarks() {
        int[][] sizes = {{50, 80}, {120, 150}, {300, 400}, {900, 700}};
        int runs = 5; // Ödev kuralı: 5 kez çalıştırıp ortalama alma

        System.out.printf("%-15s %-25s %-15s %s\n", "Language", "Implementation", "Size", "Avg. Time (ms)");
        System.out.println("-----------------------------------------------------------------------");

        for (int[] size : sizes) {
            int n = size[0], m = size[1], p = m, q = n + 10;

            // int[][] Hazırlama
            int[][] arrA = new int[n][m];
            int[][] arrB = new int[p][q];
            int[][] arrC = new int[n][q];
            
            // ArrayList Hazırlama
            ArrayList<ArrayList<Integer>> listA = new ArrayList<>();
            ArrayList<ArrayList<Integer>> listB = new ArrayList<>();
            ArrayList<ArrayList<Integer>> listC = new ArrayList<>();
            
            for(int i=0; i<n; i++) {
                ArrayList<Integer> rowA = new ArrayList<>();
                for(int j=0; j<m; j++) rowA.add(2);
                listA.add(rowA);
                
                ArrayList<Integer> rowC = new ArrayList<>();
                for(int j=0; j<q; j++) rowC.add(0);
                listC.add(rowC);
            }
            for(int i=0; i<p; i++) {
                ArrayList<Integer> rowB = new ArrayList<>();
                for(int j=0; j<q; j++) rowB.add(3);
                listB.add(rowB);
            }

            // int[][]
            long totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayDirect(arrA, arrB, arrC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "int[][] (Direct)", n + "x" + m, (totalTime / runs) / 1000000);

            // ArrayList
            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayList(listA, listB, listC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "ArrayList", n + "x" + m, (totalTime / runs) / 1000000);
        }
    }

    public static void main(String[] args) {
        if (args.length > 0) {
            // Terminalden argüman verilirse hata testi çalıştırılır
            validateFile(args[0]);
        } else {
            // Argüman yoksa performans testleri başlatılır
            System.out.println("Java Performans Olcumleri Basliyor...\n");
            runBenchmarks();
        }
    }
}