
function [C,goodpixels] = decode(imageprefix,start,stop,threshold)

% function [C,goodpixels] = decode(imageprefix,start,stop,threshold)
%
%
% Input:
%
% imageprefix : a string which is the prefix common to all the images.
%
%                  for example, pass in the prefix '/home/fowlkes/left/left_'  
%
%                  to load the image sequence   '/home/fowlkes/left/left_01.png' 
%                                               '/home/fowlkes/left/left_02.png'
%                                               '/home/fowlkes/left/left_03.png'
%                                                          etc.
%
%  start : the first image # to load
%  stop  : the last image # to load
% 
%  threshold : the pixel brightness should vary more than this threshold between the positive
%             and negative images.  if the absolute difference doesn't exceed this value, the 
%             pixel is marked as undecodeable.
%
% Output:
%
%  C : an array containing the decoded values (0..1023)  for 10bit values
%
%  goodpixels : a binary image in which pixels that were decodedable across all images are marked with a 1.

% some error checking
if (stop<=start)
  error('stop frame number should be greater than start frame number');
end

if (threshold<0)
    error('threshold must be positive');
end

bit = 1;
grayBits = (stop-start + 1)/2;
for i = start:2:stop
    first = imread(sprintf('%s%02d.png',imageprefix,i));
    if (size(first, 3) > 1)
        first = rgb2gray(first);
    end
    second = imread(sprintf('%s%02d.png', imageprefix,i+1));
    if (size(second, 3) > 1)
        second = rgb2gray(second);
    end
    
    first = im2double(first);
    second = im2double(second);

    G(:, :, bit) = first > second; %store the bits of the gray code
    goodpixels = (abs(first - second) > threshold); % good pixels at this bit level (not stored)
    badpixels(:, :, bit) = ~goodpixels; % store bad pixels at this bit level
    % visualize as we walk through the images
    figure(1); clf;
    subplot(1,2,1); imagesc(G(:,:,bit)); axis image; title(sprintf('bit %d',bit));
    subplot(1,2,2); imagesc(goodpixels); axis image; title('goodpixels');
    drawnow;

    bit = bit + 1;
end

% Calculate the good pixels for the entire image, at all bit levels,
% by AND'ing the negation of badpixels per bit level.
goodpixels(:, :) = 1;
for i = 1:grayBits
   goodpixels = goodpixels & ~badpixels(:, :, i);  
end


% convert from gray to bcd
%   remember that MSB is bit #1
BCD(:, :,1) = G(:, :, 1); % copy MSB over
for i = 2:grayBits
    BCD(:, :, i) = xor(BCD(:, :, i-1), G(:, :, i));
end


% convert from BCD to standard decimal
C = zeros(size(BCD, 1), size(BCD, 2)); % initialize C to zeros for accumulation process
for i = 1:grayBits
   C(:, :) = C(:, :) + ( 2^(grayBits-i) * BCD(:, :, i) );  % convert bit at position i to decimal value
end

% visualize final result
figure(1); clf;
subplot(1,2,1); imagesc(C.*goodpixels); axis image; title('decoded');
subplot(1,2,2); imagesc(goodpixels); axis image; title('goodpixels');
drawnow;

