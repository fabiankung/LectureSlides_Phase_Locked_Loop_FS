

function z=TF3(s,Ko,Kphi,N,C1,C2,C3,R2,R3)
    T1= (R2*C1*C2)/(C1+C2);
    T2 = R2*C2;
    T3 = R3*C3;
    F = T1*(1+s*T2)/((s*T2*C1)*(1+s*T1)*(1+s*T3));
    Num = Ko*Kphi*F/s;
    Den = 1+Num/N;
    z = Num/Den;
endfunction
