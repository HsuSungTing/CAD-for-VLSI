module c17_v4 (
    A, B, C, D, E, F, G, H, I, J, Y1, Y2, Y3, Y4, Y5, Y6, Y7
);
    input A, B, C, D, E, F, G, H, I, J ;
    output Y1, Y2, Y3, Y4, Y5, Y6, Y7 ;
    wire w1, w2, w3, w4, w5, w6, w7, w8, w9, w10;
    wire w11, w12, w13, w14, w15, w16, w17, w18, w19, w20;

    // 第一層邏輯
    NOR2X1 U1 (.A1(A), .A2(B), .ZN(w1));
    NANDX1 U2 (.A1(C), .A2(D), .ZN(w2));
    INVX1  U3 (.I(E), .ZN(w3));
    NOR2X1 U4 (.A1(F), .A2(G), .ZN(w4));
    NANDX1 U5 (.A1(H), .A2(I), .ZN(w5));
    INVX1  U6 (.I(J), .ZN(w6));

    // 第二層邏輯
    NANDX1 U7 (.A1(w1), .A2(w2), .ZN(w7));
    NOR2X1 U8 (.A1(w3), .A2(w4), .ZN(w8));
    NANDX1 U9 (.A1(w5), .A2(w6), .ZN(w9));
    NOR2X1 U10 (.A1(w2), .A2(w4), .ZN(w10));
    INVX1  U11 (.I(w7), .ZN(w11));
    
    /*wqeqwe 879n qweq we */
	/* so won't you stay with 
	oipi   poipi   poiopi 
		me cutaqweqw */

    // 第三層邏輯
    NANDX1 U12 (.A1(w8), .A2(w9), .ZN(w12));
    NOR2X1 U13 (.A1(w10), .A2(w11), .ZN(w13));
    NANDX1 U14 (.A1(w7), .A2(w12), .ZN(w14));
    INVX1  U15 (.I(w13), .ZN(w15));
    NOR2X1 U16 (.A1(w9), .A2(w14), .ZN(w16));

    // 第四層邏輯
    NANDX1 U17 (.A1(w15), .A2(w16), .ZN(w17));
    NOR2X1 U18 (.A1(w12), .A2(w17), .ZN(w18));
    NANDX1 U19 (.A1(w14), .A2(w18), .ZN(w19));
    INVX1  U20 (.I(w19), .ZN(w20));

    // 輸出級
    NANDX1 U21 (.A1(w17), .A2(w20), .ZN(Y1));
    NOR2X1 U22 (.A1(w18), .A2(w20), .ZN(Y2));
    NANDX1 U23 (.A1(w19), .A2(w16), .ZN(Y3));
    NOR2X1 U24 (.A1(w15), .A2(w12), .ZN(Y4));
    NANDX1 U25 (.A1(w13), .A2(w10), .ZN(Y5));
    NOR2X1 U26 (.A1(w8), .A2(w5), .ZN(Y6));
    NANDX1 U27 (.A1(w3), .A2(w1), .ZN(Y7));
endmodule