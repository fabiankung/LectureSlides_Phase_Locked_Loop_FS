// Routines to plot the transfer function of a closed-loop system.
// This routine is only valid for under-damped 2nd order system.
// 12 Dec 2012
// Fabian Kung

clear();

sig = 0.2; // Damping Factor.
wn = 8944.3; // Natural Frequency.
Ko = wn*wn;
a = 0.05/Ko;
b = 1;

// Finding the time constant of the system and time interval.
To = (2*(%pi))/wn;
delta_t = To/20;
Maxt = 9*To;
Maxtindex = ceil(Maxt/delta_t);

f = [10:20:100000];  // Frequency in Hz.
w = 2*(%pi)*f;       // Frequency in rad/sec.

exec('TF.sci');    // Load transfer function.   

// Finding the frequency response
for i = 1:5000 
  y(i) = TF(w(i)*(%i),Ko,a,b,wn,sig);       // Compute frequency response, output
                                            // is complex number, s= jw
  yr(i) = real(y(i));                       // Real part.
  yim(i) = imag(y(i));                      // Imaginary part.               
  mag_dB(i) = 20*log(abs(y(i)));            // Extract the magnitude in dB.
  pha_deg(i) = (180/%pi)*atan(yim(i),yr(i)); // Extract the phase in degree.
end

// Finding the time-domain response
Ct1 = (Ko*b)/(wn*wn);
Ct2 = wn*sig;
Ct3 = (sig-(a/b)*wn)/sqrt(1-sig*sig);
Ct4 = wn*sqrt(1-sig*sig);
for k = 1:Maxtindex
    t(k) = (k-1)*delta_t;
    h(k) = Ct1*(1-(exp(-Ct2*t(k)))*(Ct3*sin(Ct4*t(k))+cos(Ct4*t(k)))); 
end

// Plotting the frequency response:
subplot(221)        // Divide the graphics area into 2x2 regions
                    // and select region 1.
// Plot magnitude versus frequency
plot(f,mag_dB);
xgrid(1,2); // Add a grid to the plot, with color 1, thickness 2.
a = gca();
a.log_flags = 'lnn';
// Plot phase versus frequency
subplot(223)        // Divide the graphics area into 2x2 regions
                    // and select region 3.
plot(f,pha_deg);
xgrid(1); // Add a grid to the plot, with color 1.
a = gca();
a.log_flags = 'lnn';
// Plot imaginary versus real
subplot(222);       // Divide the graphics area into 2x2 regions
                    // and select region 2.
plot(yr,yim);
xgrid(1); // Add a grid to the plot, with color 1.

// Plot time-domain respone.
subplot(224);       // Divide the graphics area into 2x2 regions
                    // and select region 4.
plot(t,h);
xgrid(1); // Add a grid to the plot, with color 1.
