%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                           
% Main script for the loosely-coupled feedback GNSS-aided INS system. 
%  
% Edit: Isaac Skog (skog@kth.se), 2016-09-01,  
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Load data
disp('Loads data')
load('GNSSaidedINS_data.mat');

%% Load filter settings
disp('Loads settings')
settings=get_settings();

%% Run the GNSS-aided INS
disp('Runs the GNSS-aided INS')
u=[in_data.IMU.acc;in_data.IMU.gyro];
x_h=init_navigation_state(u,settings);
out(:,1)=x_h;
ctr_gnss_data=1;
ctr_speed_data=1;
out_data.delta_u_h=zeros(6,1);
[P,Q1,Q2,~,~]=init_filter(settings);
N=size(u,2);
on.x_h=zeros(10,N);
on.x_h(:,1)=x_h;
on.diag_P=zeros(15,N);
on.diag_P(:,1)=diag(P);
on.delta_u_h=zeros(6,N);
for k=2:23802
in.IMU.acc=in_data.IMU.acc(:,k);
in.IMU.gyro=in_data.IMU.gyro(:,k);
in.IMU.t=in_data.IMU.t(k);
if((rem(k,100)==1)||k==2)
in.SPEEDOMETER.speed=in_data.SPEEDOMETER.speed(ctr_speed_data);
in.SPEEDOMETER.t=in_data.SPEEDOMETER.t(ctr_speed_data);
ctr_speed_data=ctr_speed_data+1;
end

if(rem(k,100)==1||k==2)
in.GNSS.pos_ned=in_data.GNSS.pos_ned(:,ctr_gnss_data);
in.GNSS.t=in_data.GNSS.t(ctr_gnss_data);
ctr_gnss_data=ctr_gnss_data+1;
end

t_prev=in_data.IMU.t(k-1);
[out_data,P]=GPSaidedINS(in,settings,out(:,k-1),out_data.delta_u_h,P,Q1,Q2,t_prev);

out(:,k)=[out_data.x_h];
on.x_h(:,k)=out_data.x_h;
on.diag_P(:,k)=out_data.diag_P;
on.delta_u_h(:,k)=out_data.delta_u_h;

end
%% Plot the data 
plot_data(in_data,on,'True');drawnow



