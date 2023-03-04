function x_h=init_navigation_state(u,settings)


% Calculate the roll and pitch
f=mean(u(:,1:100),2);
roll=atan2(-f(2),-f(3));
pitch=atan2(f(1),norm(f(2:3)));

% Initial coordinate rotation matrix
Rb2t=Rt2b([roll pitch settings.init_heading])';

% Calculate quaternions
q=dcm2q(Rb2t);

% Initial navigation state vector
x_h=[zeros(6,1); q];

end