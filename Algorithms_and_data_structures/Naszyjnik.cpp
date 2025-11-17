#include<iostream>
#include<stdlib.h>
#include<bits/stdc++.h>

using namespace std;

    int numbers[100000];
    long long dpl[100001][10];
    long long dpr[100001][10];

int main()
{


    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n,k;
    cin>>n>>k;


    for(int i=0; i<n; ++i){
        cin>>numbers[i];
    }

    long long maxil = 0;
    long long maxir = 0;
    long long max_without_k = 0;
    
    for(int i=0; i<=n; ++i){
        for(int l = 0; l<k; ++l){

            if(i < l || i==0)
            {
                dpl[i][l]=0;
                dpr[i][l]=0;
            }
            else if(l==0){
                dpl[i][l] = maxil;
                dpr[i][l] = maxir;
            }
            else{
                dpl[i][l] = dpl[i-1][l-1] + numbers[i-1];
                dpr[i][l] = dpr[i-1][l-1] + numbers[n-i];
            }
            maxil = max(maxil, dpl[i][l]);
            maxir = max(maxir, dpr[i][l]);
        }
            max_without_k = maxil;
    }

    long long sol = 0;
    long long sum_of_k = 0;
    long long new_candidate = 0;
    for(int i = 0; i<min(k, n); ++i){
        sum_of_k+=numbers[i];
    }
        
    sol = max(0LL, sum_of_k + max(0LL, dpr[max(0,n-k)][0]));
    
    for(int i = 0; i < n-k; ++i){
        sum_of_k = sum_of_k - numbers[i] + numbers[i+k];
        new_candidate = sum_of_k + max(0LL, dpl[i+1][0]) + max(0LL, dpr[n-k-i-1][0]);
        sol = max(sol, new_candidate);
    }

    sol = max(sol, max_without_k);

    cout<<sol;

    return 0;
}