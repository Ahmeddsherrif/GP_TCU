function [P,Q1,Q2,R,H]=init_filter(settings)


% Kalman filter state matrix
P=zeros(15);
P(1:3,1:3)=settings.factp(1)^2*eye(3);
P(4:6,4:6)=settings.factp(2)^2*eye(3);
P(7:9,7:9)=diag(settings.factp(3:5)).^2;
P(10:12,10:12)=settings.factp(6)^2*eye(3);
P(13:15,13:15)=settings.factp(7)^2*eye(3);

% Process noise covariance
Q1=zeros(6);
Q1(1:3,1:3)=diag(settings.sigma_acc).^2;
Q1(4:6,4:6)=diag(settings.sigma_gyro).^2;

Q2=zeros(6);
Q2(1:3,1:3)=settings.sigma_acc_bias^2*eye(3);
Q2(4:6,4:6)=settings.sigma_gyro_bias^2*eye(3);

% GNSS-receiver position measurement noise
R=settings.sigma_gps^2*eye(3);

% Observation matrix
H=[eye(3) zeros(3,12)];

end