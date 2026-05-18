namespace HopcroftKarp;

public class HopcroftKarpMatcher
{
    private const int Nil = -1;

    private const int Inf = int.MaxValue;

    private readonly BipartiteGraph _bipartiteGraph;

    private int[] _pairLeft = Array.Empty<int>();

    private int[] _pairRight = Array.Empty<int>();

    private int[] _distance = Array.Empty<int>();

    public HopcroftKarpMatcher( BipartiteGraph bipartiteGraph )
    {
        ArgumentNullException.ThrowIfNull( bipartiteGraph );

        _bipartiteGraph = bipartiteGraph;
    }

    public Matching FindMaxChain()
    {
        int n = _bipartiteGraph.LeftSize;
        int m = _bipartiteGraph.RightSize;
        _pairLeft = new int[ n ];
        _pairRight = new int[ m ];
        _distance = new int[ n ];
        Array.Fill( _pairLeft, Nil );
        Array.Fill( _pairRight, Nil );

        while ( TryBuildLayers() )
        {
            for ( int u = 0; u < n; u++ )
            {
                if ( _pairLeft[ u ] == Nil )
                {
                    TryAugment( u );
                }
            }
        }

        return BuildResult();
    }

    private bool TryBuildLayers()
    {
        Queue<int> queue = new Queue<int>();
        for ( int u = 0; u < _bipartiteGraph.LeftSize; u++ )
        {
            if ( _pairLeft[ u ] == Nil )
            {
                _distance[ u ] = 0;
                queue.Enqueue( u );
            }
            else
            {
                _distance[ u ] = Inf;
            }
        }

        bool found = false;
        while ( queue.Count > 0 )
        {
            int u = queue.Dequeue();
            foreach ( int v in _bipartiteGraph.GetNeighbors( u ) )
            {
                int pairU = _pairLeft[ v ];
                if ( pairU == Nil )
                {
                    found = true;
                }
                else if ( _distance[ pairU ] == Inf )
                {
                    _distance[ pairU ] = _distance[ u ] + 1;
                    queue.Enqueue( pairU );
                }
            }
        }

        return found;
    }

    private bool TryAugment( int u )
    {
        foreach ( int v in _bipartiteGraph.GetNeighbors( u ) )
        {
            int pairU = _pairRight[ v ];
            bool canFollow = pairU == Nil || ( _distance[ pairU ] == _distance[ u ] + 1 && TryAugment( pairU ) );
            if ( canFollow )
            {
                _pairLeft[ u ] = v;
                _pairRight[ v ] = u;
                return true;
            }
        }

        _distance[ u ] = Inf;
        return false;
    }

    private Matching BuildResult()
    {
        Dictionary<int, int> pairs = new Dictionary<int, int>();
        for ( int i = 0; i < _bipartiteGraph.LeftSize; i++ )
        {
            if ( _pairLeft[ i ] != Nil )
            {
                pairs[ i ] = _pairLeft[ i ];
            }
        }

        return new Matching( pairs, _bipartiteGraph.LeftSize );
    }
}