import java.util.*;
import java.io.*;

class MatrixException extends Exception {
    public MatrixException(String message) {
        super(message);
    }
}

public class MatrixBenchmark {

    public static int multiplyElements(int a, int b) {
        return a * b;
    }

    public static int[][] readMatrix(Scanner scanner, int expectedRows, int expectedCols) throws MatrixException {
        int[][] matrix = new int[expectedRows][expectedCols];
        int i = 0;
        
        while (i < expectedRows) {
            if (!scanner.hasNextLine()) {
                throw new MatrixException("Satır eksik: Beklenen boyuttan daha az satır var.");
            }
            
            String line = scanner.nextLine().trim();
            if (line.isEmpty()) continue; // Boş satırları atla
            
            String[] tokens = line.split("\\s+");
            
            if (tokens.length > expectedCols) {
                throw new MatrixException("Satırda fazladan eleman: Beklenenden fazla veri girilmiş.");
            }
            if (tokens.length < expectedCols) {
                throw new MatrixException("Satırda eksik eleman: Beklenenden daha az veri girilmiş.");
            }
            
            for (int j = 0; j < expectedCols; j++) {
                try {
                    matrix[i][j] = Integer.parseInt(tokens[j]);
                } catch (NumberFormatException e) {
                    throw new MatrixException("Sayısal olmayan değer saptandı: " + tokens[j]);
                }
            }
            i++;
        }
        return matrix;
    }

    public static void validateFile(String filename) {
        try (Scanner sc = new Scanner(new File(filename))) {
            if (!sc.hasNextInt()) return;
            int n = sc.nextInt();
            int m = sc.nextInt();
            sc.nextLine(); 
            
            readMatrix(sc, n, m);

            while (sc.hasNextLine() && !sc.hasNextInt()) sc.nextLine();
            if (!sc.hasNextInt()) throw new MatrixException("Matris B boyutları okunamadı.");
            
            int p = sc.nextInt();
            int q = sc.nextInt();
            sc.nextLine();

            if (m != p) {
                throw new MatrixException("Uyumsuz matris boyutları: Matris A'nın sütun sayısı (" + m + "), Matris B'nin satır sayısına (" + p + ") eşit değil.");
            }

            readMatrix(sc, p, q);
            System.out.println(filename + " başarıyla okundu ve doğrulandı. (" + n + "x" + m + ") x (" + p + "x" + q + ")");

        } catch (FileNotFoundException e) {
            System.err.println("Dosya bulunamadı: " + filename);
        } catch (MatrixException e) {
            System.err.println("Hata (" + filename + "): " + e.getMessage());
        } catch (Exception e) {
            System.err.println("Beklenmeyen hata: " + e.getMessage());
        }
    }

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

    public static void multiplyArrayFunc(int[][] A, int[][] B, int[][] C, int n, int m, int q) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < q; j++) {
                C[i][j] = 0;
                for (int k = 0; k < m; k++) {
                    C[i][j] += multiplyElements(A[i][k], B[k][j]);
                }
            }
        }
    }

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

    public static void runBenchmarks() {
        int[][] sizes = {{50, 80}, {120, 150}, {300, 400}, {900, 700}};
        int runs = 5; 
        
        System.out.printf("%-15s %-25s %-15s %s\n", "Language", "Implementation", "Size", "Avg. Time (ms)");
        System.out.println("---------------------------------------------------------------------------");

        for (int[] size : sizes) {
            int n = size[0], m = size[1], p = m, q = n + 10;
            String sizeStr = n + "x" + m;

            int[][] arrA = new int[n][m];
            int[][] arrB = new int[p][q];
            int[][] arrC = new int[n][q];
            
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

            long totalTime = 0;

            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayDirect(arrA, arrB, arrC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "Array (Direct)", sizeStr, (totalTime / runs) / 1000000);

            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayFunc(arrA, arrB, arrC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "Array (Func Call)", sizeStr, (totalTime / runs) / 1000000);

            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayList(listA, listB, listC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "ArrayList", sizeStr, (totalTime / runs) / 1000000);
        }
    }

    public static void main(String[] args) {
        if (args.length > 0) {
            validateFile(args[0]);
        } else {
            System.out.println("Benchmarking başlıyor...\n");
            runBenchmarks();
        }
    }
}