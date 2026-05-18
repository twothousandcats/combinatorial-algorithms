namespace HopcroftKarp;

public sealed class Matching
{
    public Matching( IReadOnlyDictionary<int, int> pairs, int leftSize )
    {
        Pairs = pairs;
        IsPerfect = pairs.Count == leftSize;
    }

    public IReadOnlyDictionary<int, int> Pairs { get; }

    public bool IsPerfect { get; }
}