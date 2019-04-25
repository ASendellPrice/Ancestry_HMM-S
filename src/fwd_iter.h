#ifndef __FWD_ITER_H
#define __FWD_ITER_H

vector<mat> fwd_iter(vector<double> &recombination_rate, vector<double> &basefreq, int tt, double m, int generations, int n) 
{

    vector<double> freq(basefreq) ;
    vector<double> freq_(basefreq);
    vector<mat> transition_rates ;
    double sum ;
    double h11;
    double h12;
    double h21;
    double h22;
    double r;

    double a1;
    double a1_;

    double h11_;
    double h12_;
    double h21_;
    double h22_;

    double p_coal;

    for ( int site = 0 ; site < recombination_rate.size() ; site ++ ) {
        //for (double site = 0; site < 0.5 ; site += r) {
        r = recombination_rate[site] ;
        //vector<double> tr ;
        mat tr_mat(2,2);
        //freq_[tt+1] = m ;

        h11 = m;
        h12 = 0;
        h21 = 0;
        h22 = 1-m;

        p_coal = 1 ;

        for (int t = tt+1; t < tt + generations; t++) {
            a1 = h11 + h12;
            a1_ = h11 + h21;

            h11_ = h11*(1-r) + a1*r*a1_*p_coal;
            h12_ = h12*(1-r) + a1*r*(1-a1_)*p_coal;
            h21_ = h21*(1-r) + (1-a1)*r*a1_*p_coal;
            h22_ = h22*(1-r) + (1-a1)*r*(1-a1_)*p_coal;

            freq_[t] = ( h11_ + h21_ )/( h11 + h12 + h21 + h22 );

            h11 = h11_ ; 
            h12 = h12_ ; 
            h21 = h21_ ; 
            h22 = h22_ ; 
            
            h11 = h11/freq[t]*freq[t+1]; 
            h12 = h12/freq[t]*freq[t+1]; 
    
            h21 = h21/(1-freq[t])*(1-freq[t+1]); 
            h22 = h22/(1-freq[t])*(1-freq[t+1]); 

            sum = h11 + h12 + h21 + h22; 
            h11 = h11/sum; 
            h12 = h12/sum; 
            h21 = h21/sum; 
            h22 = h22/sum; 

            freq_[t+1] = (h11 + h21) ;
            p_coal *= ( 1 - 1/(2*n) ) ;

            //cerr << freq[t] << "," << freq_[t] << " " ;
        }
        //cerr << endl ;
        /*cout << site << " " << setprecision(15) << h11 << " " << h12 << " " << h21 << " " << h22 << " ";
        cout << h12/((h11+h12)*r) << " ";
        cout << h21/((h21+h22)*r) << " ";
        cout << h12/((h12+h22)*r) << " ";
        cout << h21/((h21+h11)*r) << " " << endl;*/
        //cerr << h12/((h11+h12)*r) << " ";
        /*tr.push_back(h12/((h11+h12)*r)) ;
        tr.push_back(h21/((h21+h22)*r)) ;
        tr.push_back(h12/((h12+h22)*r)) ;
        tr.push_back(h21/((h21+h11)*r)) ;*/
        //cerr << h12/(h11+h12) << "\t" << h21/(h21+h22) << endl;

        tr_mat(0,0) = 1 - h12/(h11+h12);
        tr_mat(0,1) = h12/(h11+h12);
        tr_mat(1,0) = h21/(h21+h22);
        tr_mat(1,1) = 1 - h21/(h21+h22);

        transition_rates.push_back(tr_mat) ;

        //cerr << r << "\t" << tr_mat(0,1) << "\t" <<  tr_mat(1,0) << endl;
        //copy(begin(freq_), end(freq_), begin(freq));
        //memcpy(freq, freq_, 300*sizeof(double));
        freq = freq_ ;
    }

    return transition_rates;
}   

mat iterate_site(mat &tr_mat, vector<double> &freqlist, double r, double site, double m, int tt, int generations) {
    double h111 = m;
    double h112 = 0;
    double h121 = 0;
    double h122 = 0;
    double h211 = 0;
    double h212 = 0;
    double h221 = 0;
    double h222 = 1 - m;
    double m11;
    double m12;
    double m21;
    double m22;
    double a1;
    double h111_;
    double h112_;
    double h121_;
    double h122_;
    double h211_;
    double h212_;
    double h221_;
    double h222_;
    double summ;

    for (int t = tt+1; t < tt + generations; t++) {
        /// get marginal haplotype frequencies
        /// Haplotype = specific combination of alleles at the two neighboring sites
        m11 = h111 + h211;
        m12 = h112 + h212;
        m21 = h121 + h221;
        m22 = h222 + h122;

        // marginal allele freqs
        // Frequency of selected allele
        a1 = (h111+h112+h121+h122);

        // now compute change associated with recombination between selected site and first marker
        h111_ = h111*(1-site) + a1*site*m11; // = h111 - site*(h111-a1*m11)
        h112_ = h112*(1-site) + a1*site*m12;
        h121_ = h121*(1-site) + a1*site*m21;
        h122_ = h122*(1-site) + a1*site*m22;
        h211_ = h211*(1-site) + (1-a1)*site*m11;
        h212_ = h212*(1-site) + (1-a1)*site*m12;
        h221_ = h221*(1-site) + (1-a1)*site*m21;
        h222_ = h222*(1-site) + (1-a1)*site*m22;

        /// then do the same thing for second set of possible recombination events
        // marginal haplotype frequencies
        m11 = h111_ + h112_;
        m12 = h121_ + h122_;
        m21 = h211_ + h212_;
        m22 = h221_ + h222_;

        // marginal allele frequencies
        a1 = h111 + h121 + h211 + h221;

        /// now second allele change associated with recombination
        h111 = h111_*(1-r) + m11*r*a1; // = h111_ - r*(h111_-m11*a1)
        h112 = h112_*(1-r) + m11*r*(1-a1);
        h121 = h121_*(1-r) + m12*r*a1;
        h122 = h122_*(1-r) + m12*r*(1-a1);
        h211 = h211_*(1-r) + m21*r*a1;
        h212 = h212_*(1-r) + m21*r*(1-a1);
        h221 = h221_*(1-r) + m22*r*a1;
        h222 = h222_*(1-r) + m22*r*(1-a1);

        // update frequencies as a consequence of selection
        h111 = h111/freqlist[t]*freqlist[t+1]; // = h111 * (freq[t+1]/freq[t])
        h112 = h112/freqlist[t]*freqlist[t+1];
        h121 = h121/freqlist[t]*freqlist[t+1];
        h122 = h122/freqlist[t]*freqlist[t+1];

        // nonselected alleles go down in frequency
        h211 = h211/(1-freqlist[t])*(1-freqlist[t+1]);
        h212 = h212/(1-freqlist[t])*(1-freqlist[t+1]);
        h221 = h221/(1-freqlist[t])*(1-freqlist[t+1]);
        h222 = h222/(1-freqlist[t])*(1-freqlist[t+1]);

        /// normalize b/c rounding errors
        summ = h111 + h112 + h121 + h211 + h122 + h212 + h221 + h222;
        h111 = h111/summ;
        h112 = h112/summ;
        h121 = h121/summ;
        h122 = h122/summ;
        h211 = h211/summ;
        h212 = h212/summ;
        h221 = h221/summ;
        h222 = h222/summ;
    }

    tr_mat(0,0) = 1 - (h112+h212)/(h111+h112+h211+h212);
    tr_mat(0,1) = (h112+h212)/(h111+h112+h211+h212);
    tr_mat(1,0) = (h121+h221)/(h121+h122+h221+h222);
    tr_mat(1,1) = 1 - (h121+h221)/(h121+h122+h221+h222);

    cerr << site << "\t" << r << "\t" << tr_mat(0,1) << "\t" <<  tr_mat(1,0) << "\t" << tr_mat(0,1)/r << "\t" << tr_mat(1,0)/r << endl;

    return tr_mat;
}

vector<mat> fwd_curve(vector<double> &recombination_rate, vector<double> &basefreq, int tt, double m, int generations) {
    vector<mat> transition_rates ;
    double r;
    mat tr_mat(2,2);
    double cumulative_r = 0;

    for ( int site = 0 ; site < recombination_rate.size() ; site ++ ) {
        r = recombination_rate[site] ;
        iterate_site(tr_mat, basefreq, r, cumulative_r, m, tt, generations);
        transition_rates.push_back(tr_mat) ;
        cumulative_r = cumulative_r + r;
    }

    return transition_rates;
}

vector<mat> approx_curve(vector<double> &recombination_rate, vector<double> &basefreq, int tt, double m, int generations) {
    mat V1_mat(2,2);
    mat V2_mat(2,2);
    mat M_mat(2,2);
    mat U_mat(2,2);
    double L1_sel;
    double L2_sel;
    double L1_non;
    double L2_non;
    double p_sel;
    double p_non;
    double alpha_sel;
    double alpha_non;
    double r_est = 0.0001;
    double x1 = 0.01;
    double x2 = 0.1;
    
    iterate_site(V1_mat, basefreq, r_est, x1, m, tt, generations);
    iterate_site(V2_mat, basefreq, r_est, x2, m, tt, generations);
    iterate_site(M_mat, basefreq, r_est, 2, m, tt, generations);
    iterate_site(U_mat, basefreq, r_est, 0, m, tt, generations);

    L1_sel = log((V1_mat(0,1)-M_mat(0,1))/(U_mat(0,1)-M_mat(0,1)));
    L2_sel = log((V2_mat(0,1)-M_mat(0,1))/(U_mat(0,1)-M_mat(0,1)));
    p_sel = log(L1_sel/L2_sel)/log(x1/x2);
    alpha_sel = -L1_sel/pow(x1,p_sel);

    L1_non = log((V1_mat(1,0)-M_mat(1,0))/(U_mat(1,0)-M_mat(1,0)));
    L2_non = log((V2_mat(1,0)-M_mat(1,0))/(U_mat(1,0)-M_mat(1,0)));
    p_non = log(L1_non/L2_non)/log(x1/x2);
    alpha_non = -L1_non/pow(x1,p_non);


    vector<mat> transition_rates ;
    double r;
    mat tr_mat(2,2);
    double cumulative_r = 0;
    double sel_trans;
    double non_trans;

    for ( int site = 0 ; site < recombination_rate.size() ; site ++ ) {
        r = recombination_rate[site] ;
        //r=0.0001;

        sel_trans = M_mat(0,1)+(U_mat(0,1)-M_mat(0,1))*exp(-alpha_sel*pow(cumulative_r,p_sel));
        non_trans = M_mat(1,0)+(U_mat(1,0)-M_mat(1,0))*exp(-alpha_non*pow(cumulative_r,p_non));

        tr_mat(0,0) = 1 -sel_trans;
        tr_mat(0,1) = sel_trans;
        tr_mat(1,0) = non_trans;
        tr_mat(1,1) = 1 - non_trans;
        
        transition_rates.push_back(tr_mat) ;

        cerr << "Approx:\t" << cumulative_r << "\t" << r << "\t" << tr_mat(0,1) << "\t" <<  tr_mat(1,0) << "\t" << tr_mat(0,1)/r << "\t" << tr_mat(1,0)/r << endl;

        cumulative_r = cumulative_r + r;
    }

    return transition_rates;
}

#endif