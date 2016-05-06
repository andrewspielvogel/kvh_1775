function out = read_kvh(file)

csv_input = csvread(file,0,1);
out.ang = csv_input(:,1:3)';
out.acc = csv_input(:,4:6)';
out.mag = csv_input(:,7:9)';
out.temp = csv_input(:,10)';
out.seq_num = csv_input(:,11)';
out.stamp = csv_input(:,12)';
out.t = out.stamp;
out.hz = round(1/(((out.stamp(end)-out.stamp(1)))/size(out.stamp,2)));