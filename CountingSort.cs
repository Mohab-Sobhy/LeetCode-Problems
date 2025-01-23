using System;

public class CountingSort
{
    public static int[] execute(int[] arrayToSort)
    {
        int[] sortedArr = new int[arrayToSort.Length];
        int[] countArr = new int[ getMaxIntInArr(arrayToSort)+1 ];

        for (int i = 0; i < arrayToSort.Length; i++)    //  O(N)
        {
            countArr[ arrayToSort[i] ]++;
        }

        for (int i = 1; i < countArr.Length; i++)   //  O(M)
        {
            countArr[i] += countArr[i - 1];
        }

        for (int i = 0; i < arrayToSort.Length; i++)    //  O(N)
        {
            int currentElement = arrayToSort[i];
            sortedArr[ countArr[currentElement]-1 ] = currentElement;
            countArr[currentElement]--;
        }

        return sortedArr;
    }
    
    private static int getMaxIntInArr(int[] arr) // O(N)
    {
        int max = int.MinValue;
        foreach (int integer in arr)
        {
            if (max < integer)
                max = integer;
        }

        return max;
    }
}

class Program
{
    static void Main(string[] args)
    {
        int[] arr = { 2,2,2,2,6,4,3,3,3,3,3,7,7 };
        int[] carr = CountingSort.execute(arr);
        foreach (int element in carr)
        {
            Console.Write(element + " ");
        }
    }
}
