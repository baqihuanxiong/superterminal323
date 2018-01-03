L = 0.18; % ǰ����������������ľ���(m)
v0 = 0.06; % ����(m/s)
beta = pi/3; % ���ת��(rad/s)
delt_t = 0.01;

diameter = 0.025; % ��ǩ��ɨ�跶Χֱ��
 
alpha = 0; % ��ʼת��
theta = pi/2; % ������̬��
xf = 0; % ǰ������x����
yf = 0-diameter/2; % ǰ������y����
xb = xf-1*L*cos(theta); % ����������x����
yb = yf-1*L*sin(theta); % ����������y����

%% ��ǩ����
c = [0 0;
    0.24 0.16;
    0.48 0.16;];

%% ת�䷽��(�����󸺣�ǰf��b)
p = [33 25 0]*pi/180;
w = ['f','f','f'];

%% ��켣
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

%% ����ǩ
for i=1:length(c)
    theta = 0:pi/50:2*pi;
    x=c(i,1)+diameter/2*cos(theta);
    y=c(i,2)+diameter/2*sin(theta);
    plot(x,y,'black-');
    hold on;
end

hold off;

%% ʶ�𵽱�ǩ
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