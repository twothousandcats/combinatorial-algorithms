using HopcroftKarp;
using Xunit;

namespace HopcroftKarpTest;

public class BipartiteGraphTests
{
    [Fact]
    public void Ctor_ZeroLeftCount_Throws()
    {
        Assert.Throws<ArgumentOutOfRangeException>( () => new BipartiteGraph( 0, 3 ) );
    }

    [Fact]
    public void Ctor_ZeroRightCount_Throws()
    {
        Assert.Throws<ArgumentOutOfRangeException>( () => new BipartiteGraph( 3, 0 ) );
    }

    [Fact]
    public void AddEdge_LeftIndexOutOfRange_Throws()
    {
        BipartiteGraph bipartiteGraph = new BipartiteGraph( 3, 3 );

        Assert.Throws<ArgumentOutOfRangeException>( () => bipartiteGraph.AddEdge( 3, 0 ) );
    }

    [Fact]
    public void AddEdge_RightIndexOutOfRange_Throws()
    {
        BipartiteGraph bipartiteGraph = new BipartiteGraph( 3, 3 );

        Assert.Throws<ArgumentOutOfRangeException>( () => bipartiteGraph.AddEdge( 0, 3 ) );
    }

    [Fact]
    public void Neighbors_AfterAddEdge_ConstrainsTarget()
    {
        BipartiteGraph bipartiteGraph = new BipartiteGraph( 3, 3 );
        bipartiteGraph.AddEdge( 0, 1 );

        IReadOnlyList<int> neighbors = bipartiteGraph.GetNeighbors( 0 );

        Assert.Equal( new[] { 1 }, neighbors );
    }
}