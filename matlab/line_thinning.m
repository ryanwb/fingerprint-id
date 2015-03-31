% Line thinning algorithm demonstration
% TODO: This! The code below is just left over from a previous lab.

set(0, 'DefaultAxesFontSize', 18);
set(0, 'DefaultAxesFontName', 'Bistream Vera Sans Mono')

% Part 1a
x = zeros(10);
y = zeros(10);
x(1) = 20;      y(1) = 172;
x(2) = 50;      y(2) = 338;
x(3) = 100;     y(3) = 441;
x(4) = 200;     y(4) = 487;
x(5) = 500;     y(5) = 499;
x(6) = 1000;	y(6) = 489;
x(7) = 2000;	y(7) = 450;
x(8) = 5000;	y(8) = 332;
x(9) = 10000;	y(9) = 219;
x(10) = 20000;	y(10) = 122;
fprintf('Frequency Response of LCDK\n');
for i = 1:10
    y(i) = 20.*log10(y(i)/503);
    fprintf('%d Hz:\t%f dB\n', x(i), y(i));
end
fprintf('\n');
figure(1);
semilogx(x, y);
title('Frequency Response of LCDK');
xlabel('Frequency (Hz)');
ylabel('Response (dB)');