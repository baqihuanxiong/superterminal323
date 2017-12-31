L = 0.16; % ǰ����������������ľ���(m)
v0 = 0.05; % ����(m/s)
beta = pi/2; % ���ת��(rad/s)
delt_t = 0.01;
 
alpha = 0; % ��ʼת��
theta = pi/2; % ������̬��
xf = 0;
yf = 0;
xb = -1*L*cos(theta);
yb = -1*L*sin(theta);

xfs = [];
xbs = [];
yfs = [];
ybs = [];
for t=0:delt_t:10
    xf = xf+v0*cos(theta-alpha)/cos(alpha)*delt_t;
    yf = yf+v0*sin(theta-alpha)/cos(alpha)*delt_t;
    xb = xb+v0*cos(theta)*delt_t;
    yb = yb+v0*sin(theta)*delt_t;
    theta = atan((yf-yb)/(xf-xb));
    if beta*t<pi/6
        alpha = beta*t;
    else
        alpha = pi/6;
    end
    xfs = [xfs xf];
    yfs = [yfs yf];
    xbs = [xbs xb];
    ybs = [ybs yb];
end

plot(xfs,yfs,'r-',xbs,ybs,'b-');
axis([-1 1 -1 1]);