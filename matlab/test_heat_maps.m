% set(0, 'DefaultAxesFontSize', 18);
% set(0, 'DefaultAxesFontName', 'Bistream Vera Sans Mono')

% Clear workspace
clear all;

images = {'russ1.JPG', 'russ2.JPG', 'russ3.JPG', ...
          'ryan1.JPG', 'ryan2.JPG', 'ryan3.JPG'};

% TODO: Probably want to do a blur around the peaks, or somehow account
% for distance! With current idea of algorithm, a distinct peak that is
% one pixel away from another peak (but instead lands on a value 0)
% gets no score for that!
      
heat_russ_one = construct_heat_map(images{1});
heat_russ_two = construct_heat_map(images{2});
heat_russ_three = construct_heat_map(images{3});
heat_russ_merged = merge_heat_maps(heat_russ_one, heat_russ_two);
heat_russ_merged = merge_heat_maps(heat_russ_merged, heat_russ_three);

figure(1);
surf(extract_ridgeendings_from_heat_map(heat_russ_merged));

heat_ryan_one = construct_heat_map(images{4});
heat_ryan_two = construct_heat_map(images{5});
heat_ryan_three = construct_heat_map(images{6});
heat_ryan_merged = merge_heat_maps(heat_ryan_one, heat_ryan_two);
heat_ryan_merged = merge_heat_maps(heat_ryan_merged, heat_ryan_three);

figure(2);
surf(extract_ridgeendings_from_heat_map(heat_ryan_merged));
