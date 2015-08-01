function [NED,w,deg] = plot_north(data,bias,N,num_samples_2_avg)

num_samples = size(data.ang);

samp = floor(num_samples(1)/num_samples_2_avg);

figure;
xlabel('x');
ylabel('y');
zlabel('z');
hold on;

%plot north
plotv(N','-c');

for i=1:samp

w(i,:) = mean(data.ang(num_samples_2_avg*(i-1)+1:num_samples_2_avg*i,1:3))-bias.ang;

NED.d(i,:) = -(mean(data.acc(num_samples_2_avg*(i-1)+1:num_samples_2_avg*i,1:3))-bias.acc);
NED.e(i,:) = cross(NED.d(i,:),w(i,:));
NED.n(i,:) = cross(NED.e(i,:),NED.d(i,:));

N = N/norm(N);
NED.n(i,:)=NED.n(i,:)/norm(NED.n(i,:));
n=NED.n(i,:);
    

true = atan2(N(2),N(1));
calc = atan2(n(2),n(1));
rad = true-calc;
    
if(rad<-pi)
    rad = rad+2*pi;
elseif(rad>pi)
    rad = rad-2*pi;    
end

deg(i) = rad*180/pi;

plot3([0,n(1)],[0,n(2)],[0,n(3)],'-b');

wn=w(i,:)/norm(w(i,:));
plot3([0,wn(1)],[0,wn(2)],[0,wn(3)],'-k');

dn=NED.d(i,:)/norm(NED.d(i,:));
plot3([0,dn(1)],[0,dn(2)],[0,dn(3)],'-r');

en=NED.e(i,:)/norm(NED.e(i,:));
plot3([0,en(1)],[0,en(2)],[0,en(3)],'-g');
end

