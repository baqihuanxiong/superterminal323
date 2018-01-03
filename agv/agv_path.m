L = 0.18; % 前读卡器与后轮轴中心距离(m)
v0 = 0.06; % 车速(m/s)
beta = pi/3; % 舵机转速(rad/s)
delt_t = 0.01;

diameter = 0.025; % 标签可扫描范围直径
 
alpha = 0; % 起始转角
theta = pi/2; % 车身姿态角
xf = 0; % 前卡中心x坐标
yf = 0-diameter/2; % 前卡中心y坐标
xb = xf-1*L*cos(theta); % 后轮轴中心x坐标
yb = yf-1*L*sin(theta); % 后轮轴中心y坐标

%% 标签坐标
c = [0 0;
    0.24 0.16;
    0.48 0.16;];

%% 转弯方向(右正左负，前f后b)
p = [33 25 0]*pi/180;
w = ['f','f','f'];

%% 求轨迹
xfs = [];
xbs = [];
yfs = [];
ybs = [];
card = 0;
t_temp = 0;

for t=0:delt_t:15
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
    
    newCardScaned = scanRfid(xf,yf,xb,yb,c,w,diameter/2);
    if newCardScaned>0&&newCardScaned~=card
        card = newCardScaned;
    end
    
    if card>0
        if alpha<p(card)
            alpha = alpha+beta*delt_t;
        else
            alpha = alpha-beta*delt_t;
        end
    end
    
    xfs = [xfs xf];
    yfs = [yfs yf];
    xbs = [xbs xb];
    ybs = [ybs yb];
end

plot(xfs,yfs,'r-',xbs,ybs,'b-');
axis([-1 1 -1 1]);
hold on;

%% 画标签
for i=1:length(c)
    theta = 0:pi/50:2*pi;
    x=c(i,1)+diameter/2*cos(theta);
    y=c(i,2)+diameter/2*sin(theta);
    plot(x,y,'black-');
    hold on;
end

hold off;

%% 识别到标签
function index=scanRfid(xf,yf,xb,yb,c,w,r)
index = 0;
for i=1:length(c)
    dist_f = sqrt((xf-c(i,1))^2+(yf-c(i,2))^2);
    dist_b = sqrt((xb-c(i,1))^2+(yb-c(i,2))^2);
    if w(i)=='f'&&dist_f<=r
        index = i;
    elseif w(i)=='b'&&dist_b<=r
        index = i;
    end
end
end