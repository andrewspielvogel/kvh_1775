function samp = gensamples(lat,hz,t_end,R_align,bias)
% gensamples(lat,hz,t_end,R_align,bias)

tic

lat = lat*pi/180;

if nargin<5
   
    bias.acc = zeros(3,1);
    bias.ang = zeros(3,1);
    
end


% Define t
dt = 1/hz;
t = 0:dt:t_end;

r = 6371*1000;

% noise
w_sig = 6.32 * 10^(-3)*pi/180;  % measured 1775, units are rad/sec
a_sig = 0.0037;            % measured 1775, units are g, not m/s^2

num = size(t,2);


samp.ang = zeros(3,num);
samp.acc = zeros(3,num);
samp.att = zeros(3,num);

samp.Rzi{1} = eye(3);

% generate a_n
Ren = [-sin(lat),0,-cos(lat);0,1,0;cos(lat),0,-sin(lat)];
a_e = [cos(lat);0;sin(lat)] - (15*pi/180/3600)^2*cos(lat)*[r;0;0]/9.81;
a_n = Ren'*a_e;

Rsz = get_Rsn(lat,0)*R_align;
samp.Rsz=Rsz;
    
fileID = fopen('data.KVH','w');

for i=1:num


    % get w_v, Rsn
    w_veh = get_w(t(i));
    Rsn = get_Rsn(lat,t(i));
    
    samp.Rsi{i} = Rsz*samp.Rzi{i};
    if i~=num
        
        w = samp.Rsi{i}'*[0;0;1]*15*pi/180/3600 + w_veh;
        samp.Rzi{i + 1} = samp.Rzi{i}*expm(skew(w)*dt);
    end

    samp.att(:,i) = rot2rph(Rsn'*samp.Rsi{i}*R_align);  
    
    samp.ang(:,i) =  w + w_sig*randn(3,1) + bias.ang;
    samp.acc(:,i) =  samp.Rsi{i}'*Rsn*a_n + a_sig*randn(3,1) + bias.acc;
    samp.acc_z(:,i) = samp.Rzi{i}*samp.acc(:,i);
   
    samp.e_v(:,i) = skew(w_veh)*samp.acc(:,i);
    % print progress
    if ~mod(t(i),30)
        str = sprintf('Made %i:%i0 of data at %i hz',floor(t(i)/60),mod(t(i),60)/10,hz);
        disp(str);
    end
    
    fprintf(fileID,'IMU_RAW, %.40f,%.40f,%.40f, %.35f,%.35f,%.35f,0,0,0, 0, 0, %.30f, 0,0,0,0,0,0, 0,0,1 \n',samp.ang(1,i),samp.ang(2,i),samp.ang(3,i),samp.acc(1,i),samp.acc(2,i),samp.acc(3,i),t(i));
        
end

% save
samp.t = t;
samp.stamp = t;
samp.hz = hz;
samp.bias = bias;
samp.noise.w_sig = w_sig;
samp.noise.a_sig = a_sig;
toc

function R = get_Rse(t)

rate = 15*pi/180/3600;

Rse = expm(skew([0,0,1])*rate*t);

R = Rse;

function R = get_Rsn(lat,t)

Ren = [-sin(lat),0,-cos(lat);0,1,0;cos(lat),0,-sin(lat)];
Rse = get_Rse(t);

R = Rse*Ren;

function w = get_w(t)

if t<5*60*0
    w=[0;0;0];
else
    
w = [cos(t/2)/40;sin(t/3)/15;cos(t/5)/10];
w=[0;0;sin(t/10)/20]*0;

end


