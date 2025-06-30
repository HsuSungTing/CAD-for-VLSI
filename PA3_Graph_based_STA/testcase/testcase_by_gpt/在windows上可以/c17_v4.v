module c17_v4 (
    N1, N2, N3, N4, N5, N6, N7, N8, N9, N10,
    N11, N12, N13, N14, N15, N16, N17, N18,N19, N20,
    N50, N51, N52, N53, N54, N55, N56, N57, N58,    N59
);
    input N1, N2, N3, N4, N5, N6, N7, N8, N9, N10;
    input N11, N12, N13, N14, N15, N16, N17, N18, N19,   N20;
    output N50, N51, N52, N53, N54, N55, N56,N57, N58,N59;
    
    wire n21, n22, n23, n24, n25, n26, n27, n28, n29,n30  ;
    wire n31, n32, n33, n34, n35, n36, n37, n38, n39, n40;
    wire n41, n42, n43, n44, n45, n46, n47, n48, n49,n60;
    wire n61, n62, n63, n64, n65, n66, n67, n68, n69,n70;
    /* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    NANDX1 U1 (.A1(N1), .A2(N2), .ZN(n21)) ;
    NOR2X1 U2 (.A1(N3), .A2(N4), .ZN(n22));
    INVX1  U3 (.I(N5), .ZN(n23));
    NANDX1 U4 (.A1(N6), .A2(N7), . ZN(n24));
    NOR2X1 U5 (.A1(N8), .A2(N9), .ZN(n25));
    INVX1  U6 (.I(N10),  .ZN(n26));
    /* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    NANDX1 U7 ( .A1(N11), .A2(N12), .ZN(n27)) ;
    NOR2X1 U8 ( .A1(N13), .A2(N14), .ZN(n28));
    INVX1  U9 (. I(N15), .ZN(n29));
    /* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    NANDX1 U10 ( .A1(N16), .A2(N17), .ZN(n30));
    NOR2X1 U11 (.A1(N18), .A2(N19), .ZN(n31));
    INVX1  U12 (.I(N20), .ZN(n32));

    NANDX1 U13 (.A1(n21), .A2(n22), .ZN(n33));
    NOR2X1 U14 (.A1(n23), .A2(n24), .ZN(n34));
    NANDX1 U15 (.A1(n25), .A2(n26), .ZN(n35));
    NOR2X1 U16 (.A1(n27), .A2(n28), .ZN(n36));
    NANDX1 U17 (.A1(n29),   .A2(n30), .ZN(n37));
    NOR2X1 U18 (.A1(n31),  .A2(n32), .ZN(n38));
    /* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

    INVX1  U19 ( .I(n33),  .ZN(n39));
    INVX1  U20 ( .I(n34), .ZN(n40));
    NANDX1 U21 (.A1(n35), .A2(n36), .ZN(n41));
    NOR2X1 U22 (. A1(n37), .A2(n38), .ZN(n42));
    /* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

    NANDX1 U23 (. A1(n39), .A2(n40) , .ZN(n43));
    NOR2X1 U24 (.A1(n41), .A2(n42), .ZN(n44));
    INVX1  U25 (. I( n43), .ZN(n45));
    NANDX1 U26 (. A1(n44), .A2(n45), .ZN(n46));

    NOR2X1 U27 (.A1 (n46), .A2(n41), .ZN(n47));
    NANDX1 U28 (.A1 (n47), .A2(n36), .ZN(n48));

    INVX1  U29 (.I(n48), .ZN(n49) );
    NANDX1 U30 (.A1( n49), .A2 (n38), .ZN(n60));

    NOR2X1 U31 (.A1(n60), .A2(n37), .ZN(n61));
    INVX1  U32 (.I(n61), .ZN(n62));

    NANDX1 U33 (.A1(n62),    .A2(n35), .ZN(n63));
    NOR2X1 U34 (.A1( n63), .A2(n34), .ZN(n64));

    NANDX1 U35 (.A1(n64), .A2(n33), .ZN(n65));
    INVX1  U36 (.I(n65 ), .ZN(n66));

    NOR2X1 U37 (.A1(n66), .A2(n32), .ZN(n67));
    NANDX1 U38 (.A1(n67), .A2(n31), .ZN(n68));

    INVX1  U39 (.I(n68), .ZN(n69));
    NOR2X1 U40 (.A1(n69), .A2(n30), .ZN(n70));
    /* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
        
    // Outputs
    INVX1  U41 (.I(n70), .ZN(N50));
    INVX1  U42 (.I(n69), .ZN(N51));
    INVX1  U43 (.I(n68), .ZN(N52));
    INVX1  U44 (.I(n67), .ZN(N53));
    INVX1  U45 (.I(n66), .ZN(N54));
    INVX1  U46 (.I(n65), .ZN(N55));
    INVX1  U47 (.I(n64), .ZN(N56));
    INVX1  U48 (.I(n63), .ZN(N57));
    INVX1  U49 (.I(n62), .ZN(N58));
    INVX1  U50 (.I(n61), .ZN(N59));

endmodule
