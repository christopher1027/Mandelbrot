# Mandelbrot
Programmed a real-time Mandelbrot generator in SystemVerilog using an Altera cyclone FPGA

Example of the first frame displayed of the mandelbrot set:
![alt text](https://github.com/christopher1027/Mandelbrot/blob/master/Mandelbrot.jpg)

The FPGA that I used:
![alt text](https://github.com/christopher1027/Mandelbrot/blob/master/altera_cyclone_fpga.jpg)

Theory:
The famous Mandelbrot set is a set of points in the complex plane.

The function is
C_{n+1} = C_{n}^2 + C_{0}

with the initial condition formed by taking the coordinates in the complex plane,
C_{0} = x + iy

FPGA Implementation:
Used an Altera_cyclone FPGA and SystemVerilog which is is a hardware description and hardware verification language for setting up the hardware design of the project

Programming:
Programmed in c to create the logic necessary to implement the Mandelbrot function.
The mandelbrot set is a set a points in the complex plane which is mapped to each pixel to display them to a computer screen.
