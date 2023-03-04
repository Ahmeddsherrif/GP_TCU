%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function call: out_data=GPSaidedINS(in_data,settings)
%
% This function integrates GNSS and IMU data. The data fusion
% is based upon a loosely-coupled feedback GNSS-aided INS approach.
%
% Input struct:
% in_data.GNSS.pos_ned      GNSS-receiver position estimates in NED
%                           coordinates [m]
% in_data.GNSS.t            Time of GNSS measurements [s]
% (in_data.GNSS.HDOP        GNSS Horizontal Dilution of Precision [-]) 
% (in_data.GNSS.VDOP        GNSS Vertical Dilution of Precision [-]) 
% in_data.IMU.acc           Accelerometer measurements [m/s^2]
% in_data.IMU.gyro          Gyroscope measurements [rad/s]
% in_data.IMU.t             Time of IMU measurements [s]
%
% Output struct:
% out_data.x_h              Estimated navigation state vector [position; velocity; attitude]
% out_data.delta_u_h        Estimated IMU biases [accelerometers; gyroscopes]
% out_data.diag_P           Diagonal elements of the Kalman filter state
%                           covariance matrix.
%
%
% Edit: Isaac Skog (skog@kth.se), 2016-09-06
% Revised: Bo Bernhardsson, 2018-01-01
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [out_data,P]=GPSaidedINS(in_data,settings,x_h,delta_u_h,P,Q1,Q2,t_prev)


% Copy data to variables with shorter name
u=[in_data.IMU.acc;in_data.IMU.gyro];
t=in_data.IMU.t;

%% Initialization
% Initialize the navigation state

% Allocate memory for the output data
out_data.x_h=zeros(10,1);
%out_data.x_h(:,1)=x_h;
out_data.diag_P=zeros(15,1);
%out_data.diag_P(:,1)=diag(P);
out_data.delta_u_h=zeros(6,1);

%% Information fusion

    % Sampling period
    Ts=t-t_prev;
    
    % Calibrate the sensor measurements using current sensor bias estimate.
    u_h=u+delta_u_h;
    
    
    % Update the INS navigation state
    x_h=Nav_eq(x_h,u_h,Ts,settings.gravity);
    
    % Get state space model matrices
    [F,G]=state_space_model(x_h,u_h,Ts);
    
    % Time update of the Kalman filter state covariance.
    P=F*P*F'+G*blkdiag(Q1,Q2)*G';
    
    % Defualt measurement observation matrix  and measurement covariance
    % matrix
    
    y1 = in_data.GNSS.pos_ned;
    y2 = in_data.SPEEDOMETER.speed;
    y3 = zeros(2,1);
    y = [y1; y2; y3];
   
      
    Rn2p = get_Rb2p()*q2dcm(x_h(7:10))';
    H1 = [eye(3) zeros(3,12)];
    H2 = [zeros(3,3) Rn2p zeros(3,9)];
    H = [H1; H2];
   
    
    R1 = [settings.sigma_gps^2*eye(3) zeros(3,3)];
    R2 = [zeros(1,3) settings.sigma_speed^2*eye(1) zeros(1,2)];
    R3 = [zeros(2,4) settings.sigma_non_holonomic^2*eye(2)];
    R = [R1; R2; R3];
%---> Hint: error standard deviations are in settings.sigma_speed and settings.sigma_non_holonomic
    
    ind = zeros(1,6); % index vector, describing available measurements     
    % Check if GNSS measurement is available
    if (t>=in_data.GNSS.t && t<in_data.GNSS.t+0.01)
        if t<settings.outagestart || t > settings.outagestop || ~strcmp(settings.gnss_outage,'on')
            ind(1:3)=1;
        end

        % Update GNSS data counter
    end
    if (t>=in_data.SPEEDOMETER.t&& t<in_data.SPEEDOMETER.t+0.01)
        if strcmp(settings.speed_aiding,'on')
            ind(4)=1;
        end

   
    end
    if strcmp(settings.non_holonomic,'on')
        ind(5:6)=1;
    end
    

    ind=logical(ind);
    H=H(ind,:);
    y=y(ind);
    R=R(ind,ind);
    
    % Calculate the Kalman filter gain.
    K=(P*H')/(H*P*H'+R);
    
    % Update the perturbation state estimate.
    z=[zeros(9,1); delta_u_h]+K*(y-H(:,1:6)*x_h(1:6));
    
    % Correct the navigation states using current perturbation estimates.
    x_h(1:6)=x_h(1:6)+z(1:6);
    x_h(7:10)=Gamma(x_h(7:10),z(7:9));
    delta_u_h=z(10:15);
    
    % Update the Kalman filter state covariance.
    P=(eye(15)-K*H)*P;
    
    % Save the data to the output data structure
    out_data.x_h=x_h;
    out_data.diag_P=diag(P);
    out_data.delta_u_h=delta_u_h;
   

end






%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%                          SUB-FUNCTIONS                                %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%       Init filter          %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%  Init navigation state     %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%  State transition matrix   %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [F,G]=state_space_model(x,u,Ts)

% Convert quaternion to DCM
Rb2t=q2dcm(x(7:10));

% Transform measured force to force in
% the tangent plane coordinate system.
f_t=Rb2t*u(1:3);
St=[0 -f_t(3) f_t(2); f_t(3) 0 -f_t(1); -f_t(2) f_t(1) 0];

% Only the standard errors included
O=zeros(3);
I=eye(3);
Fc=[O I O O O;
    O O St Rb2t O;
    O O O O -Rb2t;
    O O O O O;
    O O O O O];

% Approximation of the discret
% time state transition matrix
F=eye(15)+Ts*Fc;

% Noise gain matrix
G=Ts*[O O O O; Rb2t O O O; O -Rb2t O O; O O I O; O O O I];
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%     Error correction of quaternion    %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function q=Gamma(q,epsilon)

% Convert quaternion to DCM
R=q2dcm(q);

% Construct skew symetric matrx
OMEGA=[0 -epsilon(3) epsilon(2); epsilon(3) 0 -epsilon(1); -epsilon(2) epsilon(1) 0];

% Correct the DCM matrix
R=(eye(3)-OMEGA)*R;

% Calculte the corrected quaternions
q=dcm2q(R);
end






