module c17_v3 (N1, N2, N3, N4, N5, N6, N7, N8, N9, N10,
               N30, N31, N32, N33, N34, N35, N36, N37);
    input N1, N2, N3, N4, N5, N6, N7, N8, N9, N10;
    output N30, N31, N32, N33, N34, N35, N36, N37;
    /*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

    wire n11, n12, n13, n14, n15, n16, n17, n18, n19, n20;
    wire n21, n22, n23, n24, n25, n26, n27, n28, n29, n38;
    wire n39, n40, n41, n42, n43, n44, n45, n46, n47, n48;

    NANDX1 U1 (.A1(N1), .A2(N2), .ZN(n11));
    NOR2X1 U2 (.A1(N3), .A2(N4), .ZN(n12));
    INVX1  U3 (.I(N5), .ZN(n13));
    NANDX1 U4 (.A1(N6), .A2(N7), .ZN(n14));
    NOR2X1 U5 (.A1(N8), .A2(N9), .ZN(n15));
    INVX1  U6 (.I(N10), .ZN(n16));
    
    /*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

    NANDX1 U7 (.A1(n11), .A2(n12), .ZN(n17));
    NOR2X1 U8 (.A1(n13), .A2(n14), .ZN(n18));
    INVX1  U9 (.I(n15), .ZN(n19));
    NANDX1 U10 (.A1(n16), .A2(n17), .ZN(n20));
    
    NOR2X1 U11 (.A1(n18), .A2(n19), .ZN(n21));
    NANDX1 U12 (.A1(n20), .A2(n21), .ZN(n22));
    INVX1  U13 (.I(n22), .ZN(n23));
    
    NOR2X1 U14 (.A1(n17), .A2(n18), .ZN(n24));
    NANDX1 U15 (.A1(n19), .A2(n24), .ZN(n25));
    INVX1  U16 (.I(n20), .ZN(n26));
    
    NANDX1 U17 (.A1(n25), .A2(n26), .ZN(n27));
    NOR2X1 U18 (.A1(n23), .A2(n27), .ZN(n28));
    NANDX1 U19 (.A1(n28), .A2(n21), .ZN(n29));

    INVX1  U20 (.I(n29), .ZN(N30));
    
    NOR2X1 U21 (.A1(n28), .A2(n26), .ZN(n38));
    NANDX1 U22 (.A1(n38), .A2(n25), .ZN(N31));
    
    INVX1  U23 (.I(n38), .ZN(n39));
    NOR2X1 U24 (.A1(n39), .A2(n23), .ZN(n40));
    
    NANDX1 U25 (.A1(n40), .A2(n27), .ZN(N32));
    
    NOR2X1 U26 (.A1(n40), .A2(n19), .ZN(n41));
    NANDX1 U27 (.A1(n41), .A2(n12), .ZN(N33));
    
    INVX1  U28 (.I(n41), .ZN(n42));
    NOR2X1 U29 (.A1(n42), .A2(n18), .ZN(n43));
    
    NANDX1 U30 (.A1(n43), .A2(n11), .ZN(N34));
    
    NOR2X1 U31 (.A1(n43), .A2(n14), .ZN(n44));
    NANDX1 U32 (.A1(n44), .A2(n15), .ZN(N35));
    
    INVX1  U33 (.I(n44), .ZN(n45));
    NOR2X1 U34 (.A1(n45), .A2(n16), .ZN(n46));
    
    NANDX1 U35 (.A1(n46), .A2(n13), .ZN(N36));
    
    NOR2X1 U36 (.A1(n46), .A2(n14), .ZN(n47));
    NANDX1 U37 (.A1(n47), .A2(n19), .ZN(N37));

endmodule
