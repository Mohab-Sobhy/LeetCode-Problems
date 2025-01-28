namespace Algorithms;

public class FirstAndLastOccurrence
{
    public static int first<T>(T[] arr, T value, int left, int right) where T:IComparable<T>
    {
        int firstOccurrence = -1;
        
        while (left <= right)
        {
            int mid = ((right - left) / 2) + left;
            if (arr[mid].CompareTo(value) == 0)
            {
                firstOccurrence = mid;
                right = mid - 1;
            }
            else if (arr[mid].CompareTo(value) > 0)
            {
                right = mid - 1;
            }
            else
            {
                left = mid + 1;
            }
        }
        return firstOccurrence;
    }
}
