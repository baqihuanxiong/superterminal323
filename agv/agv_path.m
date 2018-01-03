L = 0.16; % 前读卡器与后轮轴中心距离(m)
v0 = 0.05; % 车速(m/s)
beta = pi/2; % 舵机转速(rad/s)
delt_t = 0.01;
 
alpha = 0; % 起始转角
theta = pi/2; % 车身姿态角
xf = 0;
yf = 0;
xb = -1*L*cos(theta);
yb = -1*L*sin(theta);

xfs = [];
xbs = [];
yfs = [];
ybs = [];
ths = [];
for t=0:delt_t:10
    xf = xf+v0*cos(theta-alpha)/cos(alpha)*delt_t;
    yf = yf+v0*sin(theta-alpha)/cos(alpha)*delt_t;
    xb = xb+v0*cos(theta)*delt_t;
    yb = yb+v0*sin(theta)*delt_t;
    temp_dx = xf-xb;
    if temp_dx<0
        theta = pi+atan((yf-yb)/temp_dx);
    else
        theta = atan((yf-yb)/temp_dx);
    end
    ths = [ths theta];
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