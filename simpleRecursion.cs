namespace Algorithms;

class Program
{
    private static int sumOfArr(int[] arr, int index)
    {
        if (arr.Length == index)
            return 0;

        return arr[index] + sumOfArr(arr, index + 1);
    }

    private static int Factorial(int num)
    {
        if (num == 1)
            return 1;
        return num * Factorial(num - 1);
    }

    private static int maxIntArr(int[] arr, int index)
    {
        if (arr.Length-1 == index)
            return arr[index];

        return Math.Max( arr[index], maxIntArr(arr, index + 1) );
    }
    
    static void Main(string[] args)
    {

        int[] arr = { 9, 8, 1, 3, 2, 5, 8 };
        Console.Write(sumOfArr(arr,0));
        
    }
    
}
