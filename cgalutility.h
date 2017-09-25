#ifndef CGALUTILITY_H
#define CGALUTILITY_H

#include<pch_cgal.h>

template <class Poly>
typename Poly::Halfedge_handle make_cube_3( Poly& P) {
    // appends a cube of size [0,1]^3 to the polyhedron P.
    CGAL_precondition( P.is_valid());
    typedef typename Poly::Point_3         Point;
    typedef typename Poly::Halfedge_handle Halfedge_handle;
    Halfedge_handle h = P.make_tetrahedron( Point( 1, 0, 0),
                                           Point( 0, 0, 1),
                                           Point( 0, 0, 0),
                                           Point( 0, 1, 0));
    Halfedge_handle g = h->next()->opposite()->next();
    P.split_edge( h->next());
    P.split_edge( g->next());
    P.split_edge( g);
    h->next()->vertex()->point()     = Point( 1, 0, 1);
    g->next()->vertex()->point()     = Point( 0, 1, 1);
    g->opposite()->vertex()->point() = Point( 1, 1, 0);
    Halfedge_handle f = P.split_facet( g->next(),
                                      g->next()->next()->next());
    Halfedge_handle e = P.split_edge( f);
    e->vertex()->point() = Point( 1, 1, 1);
    P.split_facet( e, f->next()->next());
    CGAL_postcondition( P.is_valid());
    return h;
}
template <class HDS>
class Polyhedron_builder : public CGAL::Modifier_base<HDS> {
public:
    std::vector<float> &coords;
    //std::vector<int> &tris;
    std::vector<int> tris;

    //Polyhedron_builder( std::vector<float> &_coords, std::vector<int> &_tris ) : coords(_coords), tris(_tris) {}
    Polyhedron_builder( std::vector<float> &_coords, std::vector<int> &_tris ) : coords(_coords){
        tris = _tris;
    }
    Polyhedron_builder( std::vector<float> &_coords, std::vector<unsigned int> &_tris_u ) : coords(_coords){
        for(int i=0;i<_tris_u.size();i++){
            tris.push_back(_tris_u[i]);
        }
    }
    void operator()( HDS& hds) {
        typedef typename HDS::Vertex   Vertex;
        typedef typename Vertex::Point Point;
        // create a cgal incremental builder
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface( coords.size()/3, tris.size()/3 );
        // add the polyhedron vertices
        for( int i=0; i<(int)coords.size(); i+=3 ){
            B.add_vertex( Point( coords[i+0], coords[i+1], coords[i+2] ) );
        }
        // add the polyhedron triangles
        for( int i=0; i<(int)tris.size(); i+=3 ){
            B.begin_facet();
            B.add_vertex_to_facet( tris[i+0] );
            B.add_vertex_to_facet( tris[i+1] );
            B.add_vertex_to_facet( tris[i+2] );
            B.end_facet();
        }
        // finish up the surface
        B.end_surface();
    }
};

#endif // CGALUTILITY_H
