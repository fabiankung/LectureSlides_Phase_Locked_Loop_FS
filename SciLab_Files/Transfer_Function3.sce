// Routines to plot the transfer function of a 4th order closed-loop system.
// 7 April 2014
// Fabian Kung

clear();

Io = 0.001;    // Charge pump current, in Amp.
Ko = 60E6; // VCO sensitivity, in Hz/Volt.
Kphi = Io/(2*(%pi));
C1 = 100E-12;   // First capacitor.
C2 = 10.0E-9;   // Shunt RC network.
C3 = 100E-12;   // Series RC network.
R2 = 10000;     // Shunt RC network.
R3 = 10000;     // Seires RC network.
N = 8500;

wn = 2*(%pi)*1000000;

// Finding the time constant of the system and time interval.
To = (2*(%pi))/wn;
delta_t = To/20;
Maxt = 9*To;
Maxtindex = ceil(Maxt/delta_t);

f = [10:20:100000];  // Frequency in Hz.
w = 2*(%pi)*f;       // Frequency in rad/sec.
exec('TF3.sci');
//exec('C:\wlkung\Google Drive\Lecture\PLL and Frequency Synthesizer\SciLab_Demo\TF3.sci');    // Load transfer function.   

// Finding the frequency response
for i = 1:5000 
  y(i) = TF3(w(i)*(%i),Ko,Kphi,N,C1,C2,C3,R2,R3);       // Compute frequency response, output
                                            // is complex number, s= jw
  yr(i) = real(y(i));                       // Real part.
  yim(i) = imag(y(i));                      // Imaginary part.               
  mag_dB(i) = 20*log(abs(y(i))/N);          // Extract the magnitude in dB.
                                            // NOTE: for frequency synthesizer we need to
                                            // normalize that gain by dividing by N.
                                            // When s goes to 0, the magnitude of the
                                            // transfer function equals N.
  pha_deg(i) = (180/%pi)*atan(yim(i),yr(i)); // Extract the phase in degree.
end


// Plotting the frequency response:
subplot(221)        // Divide the graphics area into 2x2 regions
                    // and select region 1.
// Plot magnitude versus frequency
plot(f,mag_dB);
xgrid(1);
a = gca();
a.log_flags = 'lnn';
// Plot phase versus frequency
subplot(223)        // Divide the graphics area into 2x2 regions
                    // and select region 3.
plot(f,pha_deg);
xgrid(1);
a = gca();
a.log_flags = 'lnn';
// Plot imaginary versus real
subplot(222);       // Divide the graphics area into 2x2 regions
                    // and select region 2.
plot(yr,yim);
xgrid(1);

// Plot time-domain respone.
subplot(224);       // Divide the graphics area into 2x2 regions
                    // and select region 4.
//plot(t,h);
//xgrid(1);
