module c17_v6 (
    N1, N2, N3, N4, N5, N6, N7, N8, N9, N10,
    N11, N12, N13, N14, N15, N16, N17, N18, N19, N20,
    N21, N22, N23, N24, N25, N26, N27, N28, N29, N30,
    N90, N91, N92, N93, N94, N95, N96, N97, N98, N99
);
    input N1, N2, N3, N4,N5, N6, N7, N8,N9, N10;
input N11, N12, N13,N14, N15, N16,N17, N18, N19, N20 ;
    input N21, N22, N23,N24, N25, N26,N27,N28, N29, N30 ;
    output N90, N91, N92,N93, N94,N95,N96, N97, N98, N99 ; /*wqeqwe 879n qweq we */
/*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    wire n31, n32, n33, n34, n35,n36,n37, n38, n39, n40;
wire n41, n42, n43, n44, n45,n46,   n47, n48, n49, n50;
    wire n51, n52, n53, n54, n55,n56, n57, n58, n59, n60;
    wire n61, n62, n63, n64, n65, n66,n67,   n68, n69,n70;/*wqeqwe 879n qweq we */
    wire n71, n72, n73, n74, n75, n76, n77,   n78, n79, n80; ///*wqeqwe 879n qweq we */
wire n81, n82, n83, n84, n85, n86, n87,n88, n89;
/*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    // First Layer
    NANDX1 U1 (.A1(N1), .A2(N2), .ZN(n31)) ;
    NANDX1 U2 (.A1(N3), .A2(N4), .ZN(n32)) ;
    NANDX1 U3 ( .A1(N5), .A2(N6), .ZN(n33)) ;///*wqeqwe 879n qweq we */
    NOR2X1  U4 (.A1(N7), .A2(N8), .ZN(n34));
    NOR2X1  U5 (.A1(N9), .A2(N10), .ZN(n35));
    INVX1   U6 (.I(N11), .ZN(n36))    ;
    /*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    INVX1   U7 (.I(N12), .ZN(n37));
NANDX1 U8 (.A1(N13), .A2(N14),   .ZN(n38)) ;
    NOR2X1  U9 (.A1(N15), .A2(N16),     .ZN(n39));
    NANDX1 U10 (.A1(N17), .A2(N18),   .ZN(n40));

    // Second Layer
    NOR2X1  U11 (.A1(n31), .A2(n32), .ZN(n41));
    NANDX1 U12 (.A1(n33), .A2(n34), .ZN(n42));
    NOR2X1  U13 (.A1(n35), .A2(n36), .ZN(n43));
    NANDX1 U14 (.A1(n37), .A2(n38), .ZN(n44));
    NOR2X1  U15 (.A1(n39), .A2(n40), .ZN(n45));

    // Third Layer
    NANDX1 U16 (.A1(n41), .A2(n42),  .ZN(n46));
    NOR2X1  U17 (.A1(n43), .A2(n44) ,  .ZN(n47));
    INVX1   U18 (.I(n45), .ZN(n48));
/*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    // Fourth Layer
    NANDX1 U19 (.A1(n46), .A2(n47), .ZN(n49));
    NANDX1 U20 (.A1(n48), .A2(N19), .ZN(n50));
    NOR2X1  U21 (.A1(N20), .A2(N21), .ZN(n51));
    INVX1   U22 (.I(N22), .ZN(n52));
    NANDX1 U23 (.A1(N23), .A2(N24), .ZN(n53));
    NOR2X1  U24 (.A1(N25), .A2(N26), .ZN(n54));
    NANDX1 U25 (.A1(N27), .A2(N28), .ZN(n55));
    INVX1   U26 (.I(N29), .ZN(n56));

    // Fifth Layer
    NOR2X1  U27 ( .A1(n49),  .A2(n50), .ZN(n57));
    NANDX1 U28 (.A1(n51), .A2(n52),  .ZN(n58));
    NOR2X1  U29 (.A1(n53), .A2(n54),  .ZN(n59));
    NANDX1 U30 (.A1(n55), .A2(n56), .ZN(n60)) ;

    // Sixth Layer
    NANDX1 U31 (.A1(n57), .A2(n58), .ZN(n61)) ;
    NOR2X1  U32 (.A1(n59), .A2(n60), .ZN(n62)) ;

    INVX1   U33 (.I(n61), .ZN(n63));
    NANDX1 U34 (.A1(n63), .A2(n62), .ZN(n64));
    INVX1   U35 (.I(n64), .ZN(n65));

    // Output Generation
NOR2X1  U36 (.A1(n65), .A2(N30), .ZN(n66));
    NANDX1 U37 (.A1(n66), .A2(n50), .ZN(n67));
    INVX1   U38 ( .I(n67), .ZN(n68));
    NOR2X1  U39 (.A1(n68), .A2(n49), .ZN(n69));
    NANDX1 U40 (.A1(n69), .A2(n46), .ZN(n70));
    INVX1   U41 (.I(n70), .ZN(n71));
    NOR2X1  U42 (.A1(n71), .A2(n43), .ZN(n72));
    NANDX1 U43 (.A1(n72), .A2(n41), .ZN(n73));
    INVX1   U44 (.I(n73), .ZN(n74));
    NOR2X1  U45 (.A1(n74), .A2(n33), .ZN(n75));
    NANDX1 U46 (.A1(n75), .A2(n31), .ZN(n76));
    INVX1   U47 (.I(n76), .ZN(n77));
    NOR2X1  U48 (.A1(n77), .A2(N5), .ZN(n78));
    NANDX1 U49 (.A1(n78), .A2(N3), .ZN(n79));
    INVX1   U50 (.I(n79), .ZN(n80));
    NOR2X1  U51 (.A1(n80), .A2(N1), .ZN(n81));
    NANDX1 U52 (.A1(n81), .A2(N2), .ZN(n82));
/*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */
    // Final Outputs
    INVX1   U53 (.I(n82), .ZN(N90));
    INVX1   U54 (.I(n81), .ZN(N91));
    INVX1   U55 (.I(n80), .ZN(N92));
    INVX1   U56 (.I(n79), .ZN(N93));
    INVX1   U57 (.I(n78), .ZN(N94));
    INVX1   U58 (.I(n77), .ZN(N95));
    INVX1   U59 (.I(n76), .ZN(N96));
    INVX1   U60 (.I(n75), .ZN(N97));
    INVX1   U61 (.I(n74), .ZN(N98));
    INVX1   U62 (.I(n73), .ZN(N99));

endmodule
