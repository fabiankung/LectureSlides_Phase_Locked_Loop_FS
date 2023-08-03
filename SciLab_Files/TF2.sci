// Declaration of 2nd order transfer function
// Transfer function H(s) = N(s)/D(s)
// Where N(s)=Ko(as+b)
//       D(s)=s^2+(2wn*sig*s)+(wn*wn)
//       wn = Natural frequency (rad/sec)
//       sig = Damping factor.
function y= TF2(s,Ko,a,b,wn,sig)
    
    n = Ko*(a*s+b);
    d = (s*s)+(2*wn*sig*s)+(wn*wn)
    y=n/d;
endfunction
