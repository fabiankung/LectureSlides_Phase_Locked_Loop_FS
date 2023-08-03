
function z=TF(s,Ko,a,b,wn,sig)
    N = Ko*((a*s)+b);
    D = (s*s)+(2*wn*sig*s)+(wn*wn);
    z = N/D;
endfunction
