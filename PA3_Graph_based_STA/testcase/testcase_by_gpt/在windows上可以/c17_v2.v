module c17_v2 (N1, N2, 
N3, N6, N7, N22, N23, N24, N25);
    wire n8, n9, n10,  n11, n12, n13, n14, n15, n16, n17;
    input N1, N2, N3 , N6, N7;
    /*wqeqwe 879n qweq we */
	
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    output N22, N23 , N24, N25;
    /*wqeqwe 879n qweq we */
	
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    NANDX1 U8 (.A1( N1 ),    .A2(N2), .ZN(n8));
    INVX1  U9 (.I(N3), .ZN(n9));
    NOR2X1 U10 (.A1(N6), .A2(N7),   .ZN(n10));
    NANDX1 U11 (.A1(n8),  .A2(n9),   .ZN(n11));
    NOR2X1 U12 (.A1(n9), .A2(n10), .ZN(n12));
    INVX1  U13 (.I(n11), .ZN(n13));
    NANDX1 U14 (.A1(n12), .A2(n13), .ZN(N22));
    NOR2X1 U15 (.A1(n11), .A2(n8), .ZN(n14));
    /*wqeqwe 879n qweq we */
	
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    NANDX1 U16 (.A1(n14), .A2(n12), .ZN(N23));
    INVX1  U17 (.I(n14 ) ,  .ZN(n15 ));
    NANDX1 U18 (.A1(n15), .A2(N7), .ZN(N24));
    NOR2X1 U19 (.A1(n13), .A2(n10), .ZN(n16));
    /*wqeqwe 879n qweq we */
	
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    NANDX1 U20 (.A1(n16), .A2(n9), .ZN(N25));
endmodule
