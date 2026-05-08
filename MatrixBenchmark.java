import java.util.ArrayList;

public class MatrixBenchmark {

    // Yardımcı çarpım fonksiyonu (Benchmark 2 için)
    public static int multiplyElements(int a, int b) {
        return a * b;
    }

    // Benchmark 1.1: 2D Array Doğrudan Çarpım
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

    // Benchmark 1.2: ArrayList Doğrudan Çarpım
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

    // Benchmark 2: 2D Array Fonksiyon Çağrısı ile Çarpım
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

    public static void main(String[] args) {
        int[][] sizes = {{50, 80}, {120, 150}, {300, 400}, {900, 700}};
        int runs = 5;

        System.out.printf("%-15s %-25s %-15s %s\n", "Language", "Implementation", "Size", "Avg. Time (ms)");
        System.out.println("-----------------------------------------------------------------------");

        for (int[] size : sizes) {
            int n = size[0];
            int m = size[1];
            int p = m; 
            int q = n + 10; 

            // Array Veri Üretimi
            int[][] arrA = new int[n][m];
            int[][] arrB = new int[p][q];
            int[][] arrC = new int[n][q];
            
            // ArrayList Veri Üretimi
            ArrayList<ArrayList<Integer>> listA = new ArrayList<>();
            ArrayList<ArrayList<Integer>> listB = new ArrayList<>();
            ArrayList<ArrayList<Integer>> listC = new ArrayList<>();
            
            for(int i=0; i<n; i++) {
                ArrayList<Integer> row = new ArrayList<>();
                for(int j=0; j<m; j++) row.add(2);
                listA.add(row);
                
                ArrayList<Integer> rowC = new ArrayList<>();
                for(int j=0; j<q; j++) rowC.add(0);
                listC.add(rowC);
            }
            for(int i=0; i<p; i++) {
                ArrayList<Integer> row = new ArrayList<>();
                for(int j=0; j<q; j++) row.add(3);
                listB.add(row);
            }

            long totalTime;

            // Test 1: Array Direct
            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayDirect(arrA, arrB, arrC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "Array (Direct)", n + "x" + m, (totalTime / runs) / 1000000);

            // Test 2: Array Func Call
            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayFunc(arrA, arrB, arrC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "Array (Func Call)", n + "x" + m, (totalTime / runs) / 1000000);

            // Test 3: ArrayList Direct
            totalTime = 0;
            for (int r = 0; r < runs; r++) {
                long start = System.nanoTime();
                multiplyArrayList(listA, listB, listC, n, m, q);
                totalTime += (System.nanoTime() - start);
            }
            System.out.printf("%-15s %-25s %-15s %d\n", "Java", "ArrayList", n + "x" + m, (totalTime / runs) / 1000000);
        }
    }
}