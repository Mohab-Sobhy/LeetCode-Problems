namespace Algorithms;

class Program
{
    private static int sumOfArr(int[] arr, int index)
    {
        if (arr.Length == index)
            return 0;

        return arr[index] + sumOfArr(arr, index + 1);
    }
    
    static void Main(string[] args)
    {

        int[] arr = { 9, 8, 1, 3, 2, 5, 8 };
        Console.Write(sumOfArr(arr,0));
        
    }
    
}