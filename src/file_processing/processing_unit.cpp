//
// Created by Yannick Schuermann on 07.07.24.
//

#include "processing_unit.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>

ProcessingUnit::ProcessingUnit(const std::string& csvFilePath) : csvFilePath(csvFilePath) {}

void ProcessingUnit::getRequests(size_t& numRequests, Request*& requests) const {
//create vector for storing local requests
    std::vector<Request> requestVec;
//use input file stream to read lines of csv file
    std::ifstream file(csvFilePath);

//error handling, for example if file doees not exist
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << csvFilePath << std::endl;
        numRequests = 0;
        requests = nullptr;
        return;
    }

    std::string line;
//skip header (pre lead header line before wentering loop)
    std::getline(file, line);

//main engine of class
    while (std::getline(file, line)) {
    //create string steam ss
        std::istringstream ss(line);
    //create array to store parts of the stream
        std::string parts[3];
        int i = 0;

    //split line by comma
        while (std::getline(ss, parts[i], ',')) {
            if (++i == 3) break; // read up to 3 elements
        }

    // if not exactly 3 elemtns, skip the line (maybe this should throttle the program?) <- important
        if (i != 3) {
            continue;
        }

    //store the three components
        std::string type = parts[0];
        std::string addrStr = parts[1];
        std::string dataStr = parts[2];

        try {
        // use stoul to parse adress and data values
            unsigned long addr = std::stoul(addrStr.substr(2), nullptr, 16);
            unsigned long data = std::stoul(dataStr);

        // since stoul returns unsigned long (and to my knowledge there is no method for unsigned int), we check if size can fit in uint32_t
            if (addr > std::numeric_limits<uint32_t>::max() || data > std::numeric_limits<uint32_t>::max()) {
                throw std::out_of_range("Value exceeds uint32_t limits");
            }

        //create new request object with the data parsed from the line
        // since we checked the size of our adresses and values, we can now downcast to uint32_t
            Request req;
            req.addr = static_cast<uint32_t>(addr);
            req.data = static_cast<uint32_t>(data);
            req.we = (type == "W") ? 1 : 0;
        // push to local request vector
            requestVec.push_back(req);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << line << " (" << e.what() << ")" << std::endl;
            continue;
        }
    }

    file.close();

    // after processing all requests we now can process our output

    numRequests = requestVec.size();
    requests = new Request[numRequests];

// copy values from our vector into out newly declared array
    for (size_t i = 0; i < numRequests; ++i) {
        requests[i] = requestVec[i];
    }
}
