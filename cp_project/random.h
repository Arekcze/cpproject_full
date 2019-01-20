#ifndef RANDOM_H
#define RANDOM_H
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <algorithm>
using namespace std;
class neuronNO;
typedef vector<neuronNO> layer_type;
class weights;
class passdata;
class net;
class DNA{
public:
    double learning_rate = 0.09;
    //double learning_rate = 0.001;
    double old_direction_coefficient = 0.4;

    void set_learning_rate(double passed_value){
        learning_rate = passed_value;
    }
    void set_old_difference_coefficient(double passed_value){
        old_direction_coefficient = passed_value;
    }
};

class load_data_exception : public exception
{
public:
    virtual const char* what_happened() const throw()
    {
        return "Uncorrect file pattern or file is empty";
    }

} myex;

class passdata
{
    ifstream passFile;   //getting data out of the file
public:
    passdata(const string filename) { passFile.open(filename.c_str()); };
    bool check_for_end_of_file() { return passFile.eof(); }
    void getstructure(vector<unsigned> &structure, ofstream& log_file) {
        string line;
        string pass;
        getline(passFile, line);
        stringstream ss(line);
        ss >> pass;

        try {
            if (this->check_for_end_of_file() || pass.compare("structure:") != 0)
            {
                throw myex;
            }
        }
        catch (load_data_exception& caught_exception)
        {
            log_file << caught_exception.what_happened() << endl;
            exit(0);
        }
            while (!ss.eof())
            {
                unsigned i;
                ss >> i;
                structure.push_back(i);
            }



    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned getnextinput(vector<double> &recivedvalue) {
        recivedvalue.clear();
        string line;
        getline(passFile, line);
        stringstream ss(line);
        string pass;
        ss >> pass;
        if (pass.compare("input:") == 0) {
            double randomvalue;
            while (ss >> randomvalue) { recivedvalue.push_back(randomvalue); }
        }
        return recivedvalue.size();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned getvalueaim(vector<double> &valueaim) {
        valueaim.clear();
        string line;
        getline(passFile, line);
        stringstream ss(line);
        string pass;
        ss >> pass;
        if (pass.compare("output:") == 0) {
            double randomvalue;
            while (ss >> randomvalue) {
                valueaim.push_back(randomvalue);
            }
        }
        return valueaim.size();
    }
};
class weights    //holds weights of neuronNO
{
public:
    double weight, deltaweight;
};

class neuronNO: public DNA
{
    static double activationfunction(double x)
    //{ return -tanh(x); }
    { return 1 / (1 + exp(-x)); }  // activation function
    static double derivative_of_activation_function(double x)
    {	return activationfunction(x) * (1 - activationfunction(x));} // for minst
    //{ return -(1 - activationfunction(x) *activationfunction(x)); } //derivative of this function used to measure increase/decrease in our wanted outputvalue

    static double get_random_weight(void) { return rand() / double(RAND_MAX); }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    double get_sum_of_errors_in_f_layer(const layer_type &F_layer) const {
        double sum_of_contributions_of_errors = 0.0;
        for (unsigned neuron_index = 0; neuron_index < F_layer.size() - 1; ++neuron_index) { sum_of_contributions_of_errors = sum_of_contributions_of_errors + outweights[neuron_index].weight * F_layer[neuron_index].direction_of_error; }
        return sum_of_contributions_of_errors;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    double realvalue;

    unsigned neuronindex;
    double direction_of_error;
    //double learning_rate = 0.09;
    //double learning_rate = 0.001;
    //double old_direction_coefficient = 0.4;
public:
    vector<weights> outweights;
    neuronNO(unsigned numOutputs, unsigned nindex);
    void setvalue(double passed_value) { realvalue = passed_value; }
    double getvalue(void) const { return realvalue; }
    void passdataa(const layer_type &previouslayer) {
        double sum_of_inputs_for_activation_function = 0.0;
        for (unsigned n = 0; n < previouslayer.size(); ++n)
        {
            sum_of_inputs_for_activation_function = sum_of_inputs_for_activation_function + previouslayer[n].getvalue() *  previouslayer[n].outweights[neuronindex].weight;
        }
        realvalue = neuronNO::activationfunction(sum_of_inputs_for_activation_function);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void calculate_difference_of_values_in_output_layer(double wantedvalue) {
        double difference_between_value_between_label_value_and_predicted_value = wantedvalue - realvalue;
        direction_of_error = difference_between_value_between_label_value_and_predicted_value * neuronNO::derivative_of_activation_function(realvalue);
    }
    void calculate_difference_of_values_in_hidden_layers(const layer_type &following_layer) {
        double contrubution_to_error_minimalization = get_sum_of_errors_in_f_layer(following_layer);
        direction_of_error = contrubution_to_error_minimalization * neuronNO::derivative_of_activation_function(realvalue);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void updateweights(layer_type &previouslayer);

};
void neuronNO::updateweights(layer_type &previouslayer) {//updating weighs in order to get proper results
    for (unsigned neuron_index_in_previous_layer = 0; neuron_index_in_previous_layer < previouslayer.size(); ++neuron_index_in_previous_layer)
    {
        neuronNO &single_neuron = previouslayer[neuron_index_in_previous_layer];
        double old_increase_of_weights = single_neuron.outweights[neuronindex].deltaweight;
        double new_increase_of_weights = learning_rate * single_neuron.getvalue() * direction_of_error + old_direction_coefficient * old_increase_of_weights;
        single_neuron.outweights[neuronindex].deltaweight = new_increase_of_weights;
        single_neuron.outweights[neuronindex].weight = new_increase_of_weights + single_neuron.outweights[neuronindex].weight;
    }
};
neuronNO::neuronNO(unsigned numOutputs, unsigned n_index) {
    for (unsigned c = 0; c < numOutputs; ++c) {
        outweights.push_back(weights());
        outweights.back().weight = get_random_weight();
    }
    neuronindex = n_index;
};
class net
{

    double square_error;
    double recenterror;
    double errorstamples = 100;
public:
    vector<layer_type> layers_in_a_net;
    net(const vector<unsigned> &structure);
    void propagate(const vector<double> &wantedvalue);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void passdataa(const vector<double> &recivedvalue) {
        assert(recivedvalue.size() == layers_in_a_net[0].size() - 1);
        for (unsigned i = 0; i < recivedvalue.size(); ++i) {
            layers_in_a_net[0][i].setvalue(recivedvalue[i]);
        }
        for (unsigned li = 1; li < layers_in_a_net.size(); ++li) {
            layer_type &previouslayer = layers_in_a_net[li - 1];
            for (unsigned n = 0; n < layers_in_a_net[li].size() - 1; ++n) {
                layers_in_a_net[li][n].passdataa(previouslayer);
            }
        }
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void getresult(vector<double> &presumedvalue) const {
        presumedvalue.clear();
        for (unsigned n = 0; n < layers_in_a_net.back().size() - 1; ++n) { presumedvalue.push_back(layers_in_a_net.back()[n].getvalue()); }
    }
    double calcerror(void) const { return recenterror; }

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////


net::net(const vector<unsigned> &structure) {
    unsigned numLayers = structure.size();
    for (unsigned li = 0; li < numLayers; ++li) {
        layers_in_a_net.push_back(layer_type());
        unsigned outi = li == (structure.size() - 1) ? 0 : structure[li + 1];
        for (unsigned ni = 0; ni <= structure[li]; ++ni) { layers_in_a_net.back().push_back(neuronNO(outi, ni)); }
        layers_in_a_net.back().back().setvalue(1.0);
    }
};
//functions added outside of the class to look more classlike
void net::propagate(const vector<double> &wantedvalue) {
    layer_type &outputlayer = layers_in_a_net.back();
    square_error = 0.0;
    for (unsigned neuron_index = 0; neuron_index < outputlayer.size() - 1; ++neuron_index)
    {
        double difference_between_wanted_value_and_labeled_value = wantedvalue[neuron_index] - outputlayer[neuron_index].getvalue();
        square_error = square_error + difference_between_wanted_value_and_labeled_value * difference_between_wanted_value_and_labeled_value;
    }
    square_error = square_error / (outputlayer.size() - 1);
    square_error = sqrt(square_error);
    recenterror = (recenterror * errorstamples + square_error) / (errorstamples + 1.0);
    for (unsigned n = 0; n < outputlayer.size() - 1; ++n)
    {
        outputlayer[n].calculate_difference_of_values_in_output_layer(wantedvalue[n]);
    }
    for (unsigned layer_index = layers_in_a_net.size() - 2; layer_index > 0; --layer_index)
    {
        layer_type &hiddenlayer = layers_in_a_net[layer_index];
        layer_type &following_layer = layers_in_a_net[layer_index + 1];
        for (unsigned n = 0; n < hiddenlayer.size(); ++n) { hiddenlayer[n].calculate_difference_of_values_in_hidden_layers(following_layer); }
    }

    for (unsigned li = layers_in_a_net.size() - 1; li > 0; --li)
    {
        layer_type &layer = layers_in_a_net[li];
        layer_type &previouslayer = layers_in_a_net[li - 1];
        for (unsigned n = 0; n < layer.size() - 1; ++n) { layer[n].updateweights(previouslayer); }
    }
};

template <class T, class K>
void printvalue(T pass, K &v, ofstream& myfile)
{
    myfile << pass << " ";
    for (unsigned i = 0; i < v.size(); ++i) { myfile << v[i] << " "; }
    myfile << endl;
}
template <class T, class K>
void printvalue2(T pass, K &v, ofstream& myfile)
{
    myfile << pass << " ";
    for (unsigned i = 0; i < v.size(); ++i) {
        if (i % 28 == 0) {
            myfile << endl;
        }
        myfile << v[i] << " "; }
    myfile << endl;
}
void print_weights(net& randomnet, ofstream& file){
    for (int i = 0;i<randomnet.layers_in_a_net.size()-1 ;i++) {
        file << "layer nr"<<i<<": ";
                for(int j = 0; j<randomnet.layers_in_a_net.at(i).size() ;j++){
                   file <<endl<<"neuron nr."<<j<<": "<<endl;
                    for(int g = 0; g<randomnet.layers_in_a_net.at(i).at(j).outweights.size() ;g++)
                    file << randomnet.layers_in_a_net.at(i).at(j).outweights.at(g).weight<< " ";
        }
                file << endl;
    }
    file << "deltaweigts";
    for (int i = 0;i<randomnet.layers_in_a_net.size()-1 ;i++) {
        file <<endl<< "layer nr"<<i<< ": ";
                for(int j = 0; j<randomnet.layers_in_a_net.at(i).size() ;j++){
                    file<<endl <<"neuron nr."<<j<<": "<<endl;
                    for(int g = 0; g<randomnet.layers_in_a_net.at(i).at(j).outweights.size() ;g++)
            file << randomnet.layers_in_a_net.at(i).at(j).outweights.at(g).deltaweight<< " ";
        }
    }

}

int run_neural_net(string input,string input2, string input_file)
{
    srand(time(NULL));
    ofstream file_for_logs;
    file_for_logs.open(input);
    passdata a_passdata_object(input_file);
    vector<unsigned> structure_from_file;
    a_passdata_object.getstructure(structure_from_file, file_for_logs);
    net randomnet(structure_from_file);
    vector<double> data_from_file, wanted_outcome, data_predicted_by_net;
    int i = 0;
    while (!a_passdata_object.check_for_end_of_file())
    {
        ++i;
        file_for_logs << "Pass count" << i << endl;
        if (a_passdata_object.getnextinput(data_from_file) != structure_from_file[0])
            break;
        printvalue2("Collected data :", data_from_file, file_for_logs);
        randomnet.passdataa(data_from_file);
        randomnet.getresult(data_predicted_by_net);
        printvalue("Predicted data:", data_predicted_by_net, file_for_logs);
        a_passdata_object.getvalueaim(wanted_outcome);
        printvalue("Wanted outcome:", wanted_outcome, file_for_logs);
        assert(wanted_outcome.size() == structure_from_file.back());

        randomnet.propagate(wanted_outcome);

        file_for_logs << "Netrms: "
            << randomnet.calcerror() << endl;
    }
    //string input3= "C:\\Users\\arek\\Documents\\lel\\xd10.txt"
;   file_for_logs << endl << "Program has ended." << endl;
    file_for_logs.close();
    ofstream file_for_weights;
    file_for_weights.open(input2);
    print_weights(randomnet, file_for_weights);
    file_for_weights.close();
    return 0;
}

#endif // RANDOM_H
