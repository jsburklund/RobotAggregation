clear
syms l dt V d r
assume(l > 0)
assume(dt > 0)
assume(V > 0)
assume(d > 0)
assume(r > 0)
assumeAlso(d > sqrt(3)*r)

Ri = l;
omegai = (2*V)/(3*l);
thetai = omegai*dt;

assumeAlso(thetai < pi)

D = sqrt(d^2 + (-r)^2);
D_hat = sqrt((d + 1/5*Ri*sin(thetai)*sin(5/4*thetai) - sin(thetai)*Ri)^2 + (-r-1/5*Ri*cos(thetai)*sin(5/4*thetai) - (Ri - cos(thetai)*Ri))^2);
D_hat = simplify(D_hat);
latex(D_hat <= D)