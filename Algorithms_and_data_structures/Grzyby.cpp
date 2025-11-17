# include <iostream>
# include <vector>

# define MODULO 1000000000

using namespace std;

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n,m,k,g;
    cin>>n>>m>>k>>g;

    vector<vector<bool>> forest(n, vector<bool>(m, false));
    vector<vector<vector<long long>>> dp_left(n, vector<vector<long long>>(m, vector<long long>(11, 0)));
    vector<vector<vector<long long>>> dp_below(n, vector<vector<long long>>(m, vector<long long>(11, 0)));
    vector<vector<vector<long long>>> dp_up(n, vector<vector<long long>>(m, vector<long long>(11, 0)));

    int x,y;
    for (int i=0; i<g; ++i) {
        cin>>y>>x;
        forest[y-1][x-1]=true;
    }

    if(!forest[0][0])
    {
        dp_left[0][0][0]=1;
    }
    else
    {
        dp_left[0][0][1]=1;
    }

    for (int j=0; j<m; ++j) {
        for (int i=0; i<n; ++i) {
            if(j>0)
            {
                if(!forest[i][j])
                {
                    for (int l=0; l<11; ++l) {
                        dp_left[i][j][l]=(dp_left[i][j-1][l]+dp_below[i][j-1][l]+dp_up[i][j-1][l])%MODULO;
                    }
                }
                else
                {
                    for (int l=1; l<11; ++l) {
                        dp_left[i][j][l]=(dp_left[i][j-1][l-1]+dp_below[i][j-1][l-1]+dp_up[i][j-1][l-1])%MODULO;
                    }
                    dp_left[i][j][10]+=(dp_left[i][j-1][10]+dp_below[i][j-1][10]+dp_up[i][j-1][10])%MODULO;
                }
            }
            if(i>0)
            {
                if(!forest[i][j])
                {
                    for (int l=0; l<11; ++l) {
                        dp_up[i][j][l]=(dp_left[i-1][j][l]+dp_up[i-1][j][l])%MODULO;
                    }                
                }
                else
                {
                    for (int l=1; l<11; ++l) {
                       dp_up[i][j][l]=(dp_left[i-1][j][l-1]+dp_up[i-1][j][l-1])%MODULO;
                    }
                    dp_up[i][j][10]+=(dp_left[i-1][j][10]+dp_up[i-1][j][10])%MODULO;
                }
            }
        }

        for (int i=n-1; i>=0; --i) {
            if (i<=n-2) {
                if(!forest[i][j])
                {
                    for (int l=0; l<11; ++l) {
                        dp_below[i][j][l]=(dp_left[i+1][j][l]+dp_below[i+1][j][l])%MODULO;
                    }    
                }
                else
                {
                    for (int l=1; l<11; ++l) {
                        dp_below[i][j][l]=(dp_left[i+1][j][l-1]+dp_below[i+1][j][l-1])%MODULO;
                    }
                    dp_below[i][j][10]+=(dp_below[i+1][j][10]+dp_left[i+1][j][10])%MODULO;
                }
            }
        }
    }

    long long sol=0;
    for (int i=10; i>=k; --i) {
        sol+=(dp_left[n-1][m-1][i]+dp_below[n-1][m-1][i]+dp_up[n-1][m-1][i])%MODULO;
        sol%=MODULO;
    }
    cout<<sol;
    
    return 0;
}