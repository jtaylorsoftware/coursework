function X = triangulate(xL,xR,camL,camR)

%
%  function X = triangulate(xL,xR,camL,camR)
%
%  INPUT:
%   
%   xL,xR : points in left and right images  (2xN arrays)
%   camL,camR : left and right camera parameters
%
%
%  OUTPUT:
%
%    X : 3D coordinate of points in world coordinates (3xN array)
%
%

% Error check the left points and left camera
%   Do error checking on input matrix
if size(xL, 1) ~= 2
    error('triangulate(xL,xR,camL,camR) error: xL is not a 2xN matrix')
end

% Error check the right points and camera
%   Do error checking on input matrix
if size(xR, 1) ~= 2
    error('triangulate(xL,xR,camL,camR) error: xR is not a 2xN matrix')
end
    
N = size(xR, 2);
% 1. convert xL and xR from pixel coordinates back into meters with unit focal 
% length by subtracting off principal point and dividing through by 
% focal length...  call the results qR and qL.
qL(1,:) = (xL(1,:) - camL.c(1)) / camL.f;
qL(2,:) = (xL(2,:) - camL.c(2)) / camL.f;
qL(3,:) = 1;

qR(1,:) = (xR(1,:) - camR.c(1)) / camR.f;
qR(2,:) = (xR(2,:) - camR.c(2)) / camR.f;
qR(3,:) = 1;

% 2. make the right camera the origin of the world coordinate system by 
% transforming both cameras appropriately in order to find the rotation
% and translation (R,t) relating the two cameras
R = inv(camR.R) * camL.R;
t = inv(camR.R) * (camL.t - camR.t);

% 3. Loop over each pair of corresponding points qL,qR and 
% solve the equation:  
%
%   Z_R * qR = Z_L * R * qL + t
%
% for the depth values Z_R and Z_L using least squares.
%
Z_R = zeros(1, N);
for n = 1:N
    A = [qR(:, n) -R*qL(:, n)]; % known values
    u = A \ t; % solve using least squares
    Z_R(n) = u(1); % Z_R is first value of u
end
% 4. use Z_R to compute the 3D coordinates XR = (X_R,Y_R,Z_R) in right camera
% reference frame
XR = bsxfun(@times, qR, Z_R); % multiply each XR column vector by Z_R

% 5. since the right camera wasn't at the origin, map XR back to world coordinates 
% X using the right camera transformation parameters.
X = camR.R * XR + camR.t;


