#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main()
{

    std::ios_base::sync_with_stdio( false );
    std::cin.tie( NULL );

    int n,m;
    cin>>n>>m;
    vector<int> positions;
    vector<int> robots;
    vector<long long> tmp(2*n+1, 0);
    int x;

    for(int i=0; i<n; ++i) {
        cin>>x;
        robots.push_back(x);
        tmp[0]+=x;
    }

    for(int i=0; i<n; ++i) {
        cin>>x;
        positions.push_back(x);
        positions.push_back(x-m);
        positions.push_back(x+m);
    }

    sort(positions.begin(), positions.end());
    sort(robots.begin(), robots.end());

    int finder=0;
    for(int i=0; i<n; ++i) {
        while(finder < 3*n && positions[finder]<robots[i]){
            finder++;
        }
        tmp[finder-i]-=2*robots[i];
    }

    finder = 0;
    for(int i = 0; i < 3*n ; ++i) {
        while(i>=n && i<2*n && finder < n && positions[i]>=robots[finder]){
            finder++;
        }
        if(i<n)
        {
            tmp[0]-=positions[i];
            tmp[i+1]+=positions[i];
        }
        else if(i>=n && i<2*n && finder < n)
        {
            tmp[i-n+1]-=positions[i];
            tmp[i - finder +1]+=2*positions[i];
            tmp[i+1]-=positions[i];
        }
        else
        {
            tmp[i-n+1]+=positions[i];
            if(i<2*n){
                tmp[i+1]-=positions[i];
            }
        }
    }

    long long sol=tmp[0];
    for(int i = 1; i < tmp.size(); ++i) {
        tmp[i]+=tmp[i-1];
        if(tmp[i] < sol)
        sol = tmp[i];
    }

    cout<<sol;

    return 0;
}