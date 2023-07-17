module top ( x0 , x1 , y0 , y1 , z , u0 , u1 , w0 , w1 ) ;
    input x0 , x1 , y0 , y1 , z , const ;
    output u0 , u1 , w0 , w1 ;
    wire n1 , n2 , n3 , n4 , n5 , n6 ;
    and g1 ( n1 , x1 , y0 ) ;
    xor g8 ( n4 , n1 , const );
    and g2 ( n2 , x0 , y1 ) ;
    xor g9 ( n5 , n2 , const );
    and g3 ( n3 , y0 , z ) ;
    xor g10 ( n6 , n3 , const );
    and g4 ( u0 , y0 , x0 ) ;
    xor g5 ( u1 , n4 , n5 ) ;
    xnor g6 ( w0 , y1 , n6 ) ;
    xor g7 ( w1 , y0 , z ) ;
endmodule
