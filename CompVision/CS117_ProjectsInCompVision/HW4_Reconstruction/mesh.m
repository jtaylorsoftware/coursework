%
% load results of reconstruction
%
load reconstruction


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% cleaning step 1: remove points outside known bounding box
%

% set thresholds in each direction to allow fine tuning
POSXTHRESH = 400;
NEGXTHRESH = -400;
POSYTHRESH = 400;
NEGYTHRESH = -400;
POSZTHRESH = 200;
NEGZTHRESH = -180;

% calculate points within the threshold and drop bad points
validX = find(NEGXTHRESH < X(1, :) & X(1, :) < POSXTHRESH); 
X = X(:, validX); % prune invalid X's 
xL = xL(:, validX);
xR = xR(:, validX);

validY = find(NEGYTHRESH < X(2, :) & X(2, :) < POSYTHRESH);
X = X(:, validY); % next prune invalid Y's
xL = xL(:, validY);
xR = xR(:, validY);

validZ = find(NEGZTHRESH < X(3, :) & X(3, :) < POSZTHRESH);
X = X(:, validZ); % finally prune invalid Z's
xL = xL(:, validZ);
xR = xR(:, validZ);

tri = delaunay(xL(1, :), xL(2, :)); %do triangulation
% check result of bounding box pruning


figure(1); clf;
colormap default
h = trisurf(tri,X(1,:), X(2,:), X(3,:));
set(h,'edgecolor','none')
set(gca,'projection','perspective')
axis image; axis vis3d;
view(0, -60)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% cleaning step 2: remove triangles which have long edges
%


% Get the user defined threshold
TRITHRESH = 0;   %10mm
while (TRITHRESH <= 0) || ~isnumeric(TRITHRESH)
    answer = inputdlg('Enter the threshold for the length of a triangle side:');
    
    
    if size(answer) == 0
        error('User hit cancel on TRITHRESH input');
    elseif isnan(str2double(answer{1}))
        uiwait(errordlg('Please enter a positive numeric value'));    
    else
        TRITHRESH = str2double(answer{1});
    end
end

%
% remove unreferenced points which don't appear in any triangle
%

% Get a 3xNumTris matrix for each point of each triangle in the
% triangulation

points1 = X(:, tri(:, 1));
points2 = X(:, tri(:, 2));
points3 = X(:, tri(:, 3));

% Calculate the length of each side for every triangle 
side1 = vecnorm(abs(points1 - points2));
side2 = vecnorm(abs(points1 - points3));
side3 = vecnorm(abs(points2 - points3));

% Get the maximum side length per triangle in a 3xNumTri matrix
sides = [side1; side2; side3];
maxSides = max(sides, [], 1);

% Find the indices of the valid triangle sides
validSides = find(maxSides < TRITHRESH);


tri = tri(validSides, :); % Prune triangles that fell outside threshold (by keeping valid ones)
validIndices = unique(tri);
validX = X(:, validIndices); % prune points not in triangulation by keeping valid points

% Remap the points that are still part of the triangulation to their
% correct indices

[~, indxX, indxValidX] = intersect(X', validX', 'rows'); % create a mapping from old index -> new index
[indxTri, indxOldX] = ismember(tri, indxX); % find the indices in tri with the old X index
tri(indxTri) = indxValidX(indxOldX(indxTri)); % update the values in tri to the valid ones

X = validX;
xL = xL(:, validIndices);
xR = xR(:, validIndices);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% display results
%
figure(2); 
clf;
subplot(1, 2, 1);
h = trisurf(tri,X(1,:),X(2,:),X(3,:));
set(h,'edgecolor','none')
set(gca,'projection','perspective')
axis image; axis vis3d;

% rotate the view around so we see from
% the front  (can also do this with the mouse in the gui)
view(0, -60)

subplot(1, 2, 2)
h = trisurf(tri,X(1,:),X(2,:),X(3,:));
set(h,'edgecolor','none')
set(gca,'projection','perspective')
axis image; axis vis3d;

% rotate to see from behind
view(180, 90)

%
% MATLAB has other interesting options to control the
% rendering of the mesh... see e.g.
%
%  lighting flat;
%  shading interp;
%  material shiny;
%  camlight headlight;
%
%
