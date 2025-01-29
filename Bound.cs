namespace Algorithms;

public class Bound
{
    public static int lower<T>(T[] arr, T target) where T : IComparable<T>
    {
        int low = 0;
        int high = arr.Length - 1;
        int answer = arr.Length;

        while (high >= low)
        {
            int mid = ((high - low) / 2) + low;
            if (arr[mid].CompareTo(target) >= 0)    // arr[mid] >= target
            {
                answer = mid;
                high = mid - 1;
            }
            else
            {
                low = mid + 1;
            }
        }
        
        return answer;
    }
    
    public static int upper<T>(T[] arr, T target) where T : IComparable<T>
    {
        int low = 0;
        int high = arr.Length - 1;
        int answer = arr.Length;

        while (high >= low)
        {
            int mid = ((high - low) / 2) + low;
            if (arr[mid].CompareTo(target) > 0)    // arr[mid] == target
            {
                answer = mid;
                high = mid - 1;
            }
            else
            {
                low = mid + 1;
            }
        }
        
        return answer;
    }
    
}
