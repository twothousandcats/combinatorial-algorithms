using HopcroftKarp;
using Xunit;

namespace HopcroftKarpTest;

public class HopcroftKarpMatcherTests
{
    [Fact]
    public void Ctor_NullGraph_Throws()
    {
        Assert.Throws<ArgumentNullException>( () => new HopcroftKarpMatcher( null! ) );
    }

    [Fact]
    public void FindMaxChain_NoEdges_ReturnsEmpty()
    {
        BipartiteGraph bipartiteGraph = new BipartiteGraph( 3, 3 );
        HopcroftKarpMatcher hopcroftKarpMatcher = new HopcroftKarpMatcher( bipartiteGraph );

        Matching result = hopcroftKarpMatcher.FindMaxChain();

        Assert.Empty( result.Pairs );
        Assert.False( result.IsPerfect );
    }

    [Fact]
    public void FindMaxChain_SingleEdge_MatchesOnePair()
    {
        BipartiteGraph bipartiteGraph = new BipartiteGraph( 2, 2 );
        bipartiteGraph.AddEdge( 0, 1 );
        HopcroftKarpMatcher hopcroftKarpMatcher = new HopcroftKarpMatcher( bipartiteGraph );

        Matching result = hopcroftKarpMatcher.FindMaxChain();

        Assert.Single( result.Pairs );
        Assert.Equal( 1, result.Pairs[ 0 ] );
        Assert.False( result.IsPerfect );
    }

    [Fact]
    public void FindMaxChain_PerfectMatchExists_ReturnsCorrectIsPerfect()
    {
        BipartiteGraph bipartiteGraph = new BipartiteGraph( 3, 3 );
        bipartiteGraph.AddEdge( 0, 0 );
        bipartiteGraph.AddEdge( 0, 1 );
        bipartiteGraph.AddEdge( 1, 0 );
        bipartiteGraph.AddEdge( 1, 2 );
        bipartiteGraph.AddEdge( 2, 1 );
        HopcroftKarpMatcher hopcroftKarpMatcher = new HopcroftKarpMatcher( bipartiteGraph );

        Matching result = hopcroftKarpMatcher.FindMaxChain();

        Assert.Equal( 3, result.Pairs.Count );
        Assert.True( result.IsPerfect );
    }
}