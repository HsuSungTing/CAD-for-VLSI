module c17_v5 (
    N1, N2, N3, N4, N5, N6, N7, N8, N9, N10,
    N11, N12, N13, N14, N15,N16, N17, N18, N19, N20,

    N21, N22, N23, N24, N25,
    N80, N81, N82, N83, N84, N85, N86, N87,N88, N89
);
    input N1, N2, N3, N4, N5, N6,N7,N8,N9,N10 ;
    input N11, N12, N13, N14, N15, N16,N17, N18, N19, N20  ;
    input N21, N22, N23,N24,N25;
    output N80, N81, N82, N83, N84,N85, N86, N87, N88, N89  ;

    wire n26, n27, n28, n29, n30, n31, n32,n33, n34, n35  ;
    wire n36, n37, n38, n39, n40, n41, n42,n43, n44, n45;
    wire n46, n47, n48, n49, n50, n51, n52, n53, n54, n55;
    wire n56, n57, n58, n59, n60, n61, n62, n63, n64, n65;
    wire n66, n67, n68, n69, n70, n71, n72, n73, n74, n75;

    // First Layer
    NANDX1 U1 (.A1(N1), .A2(N2),  .ZN(n26) );
    NANDX1 U2 (.A1(N3), .A2(N4), .ZN(n27));
    NANDX1 U3 (.A1(N5), .A2(N6), .ZN(n28));
    NANDX1 U4 (.A1(N7), .A2(N8), .ZN(n29));
    NANDX1 U5 (.A1(N9), .A2(N10), .ZN(n30));
    NANDX1 U6 (.A1(N11), .A2(N12), .ZN(n31));
    NANDX1 U7 (.A1(N13), .A2(N14), .ZN(n32));
    NANDX1 U8 (.A1(N15), .A2(N16), .ZN(n33));
    NANDX1 U9 (.A1(N17), .A2(N18), .ZN(n34));
    NANDX1 U10 (.A1(N19), .A2(N20), .ZN(n35));

    // Second Layer
    NANDX1 U11 (.A1(n26), .A2(n27), .ZN(n36));
    NANDX1 U12 (.A1(n28), .A2(n29), .ZN(n37)) ;
    NANDX1 U13 (.A1(n30), .A2(n31), .ZN(n38))   ;
    NANDX1 U14 (.A1(n32), .A2(n33), .ZN(n39));
    NANDX1 U15 (.A1(n34), .A2(n35), .ZN(n40));

    // Third Layer
    NANDX1 U16 (.A1(n36), .A2(n37), .ZN(n41));
    NANDX1 U17 (.A1(n38), .A2(n39), .ZN(n42));
    NANDX1 U18 (.A1(n40), .A2(N21), .ZN(n43));
    NOR2X1  U19 (.A1(N22), .A2(N23), .ZN(n44))   ;
    INVX1   U20 (.I(N24), .ZN(n45));

    // Fourth Layer
    NANDX1 U21 (  .A1(n41), .A2(n42), .ZN(n46));
    NANDX1 U22 (.A1(n43), .A2(n44), .ZN(n47));
    NANDX1 U23 (.A1(n45), .A2(N25), .ZN(n48));

    // Fifth Layer
    NANDX1 U24 (.A1(n46), .A2(n47), .ZN(n49));
    NOR2X1  U25 (.A1(n48), .A2(n44), .ZN(n50));

    // Sixth Layer
    NANDX1 U26 (.A1(n49), .A2(n50), .ZN(n51));

    INVX1   U27 (.I(n51), .ZN(n52));
    NANDX1 U28 (.A1(n52), .A2(n48), .ZN(n53));
    INVX1   U29 (.I(n53), .ZN(n54));

    NOR2X1  U30 (.A1(n54), .A2(n47), .ZN(n55));
    NANDX1 U31 (.A1(n55), .A2(n46), .ZN(n56));

    INVX1   U32 (.I(n56), .ZN(n57));
    NOR2X1  U33 (.A1(n57), .A2(n43), .ZN(n58));

    NANDX1 U34 (.A1(n58), .A2(n42), .ZN(n59));
    INVX1   U35 (.I(n59), .ZN(n60));

    NOR2X1  U36 (.A1(n60), .A2(n41), .ZN(n61));
    NANDX1 U37 (.A1(n61), .A2(n36), .ZN(n62));

    INVX1   U38 (.I(n62), .ZN(n63));
    NOR2X1  U39 (.A1(n63), .A2(n30), .ZN(n64));

    NANDX1 U40 (.A1(n64), .A2(n28), .ZN(n65));
    INVX1   U41 (.I(n65), .ZN(n66));

    NOR2X1  U42 (.A1(n66), .A2(n26), .ZN(n67));

    // Outputs
    INVX1   U43 (.I(n67), .ZN(N80));
    INVX1   U44 (.I(n66), .ZN(N81));
    INVX1   U45 (.I(n65), .ZN(N82));
    INVX1   U46 (.I(n64), .ZN(N83));
    INVX1   U47 (.I(n63), .ZN(N84));
    INVX1   U48 (.I(n62), .ZN(N85));
    INVX1   U49 (.I(n61), .ZN(N86));
    INVX1   U50 (.I(n60), .ZN(N87));
    INVX1   U51 (.I(n59), .ZN(N88));
    INVX1   U52 (.I(n58), .ZN(N89));

endmodule