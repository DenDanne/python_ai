#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class DecisionTree {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        if (x[88] <= -0.031000000424683094) {
                            if (x[75] <= 0.017500000074505806) {
                                return 1;
                            }

                            else {
                                if (x[186] <= 0.10749999806284904) {
                                    return 0;
                                }

                                else {
                                    return 1;
                                }
                            }
                        }

                        else {
                            if (x[193] <= 0.07750000059604645) {
                                if (x[77] <= -0.021499999798834324) {
                                    if (x[372] <= -0.007999999914318323) {
                                        if (x[520] <= -0.19400000176392496) {
                                            return 1;
                                        }

                                        else {
                                            return 3;
                                        }
                                    }

                                    else {
                                        return 0;
                                    }
                                }

                                else {
                                    if (x[171] <= 0.022499999962747097) {
                                        if (x[15] <= -0.020500000566244125) {
                                            if (x[450] <= -0.001999999978579581) {
                                                return 0;
                                            }

                                            else {
                                                return 1;
                                            }
                                        }

                                        else {
                                            return 2;
                                        }
                                    }

                                    else {
                                        if (x[135] <= 0.026499999687075615) {
                                            if (x[114] <= -0.07500000204890966) {
                                                return 1;
                                            }

                                            else {
                                                return 0;
                                            }
                                        }

                                        else {
                                            return 3;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[72] <= -0.03799999970942736) {
                                    return 1;
                                }

                                else {
                                    return 3;
                                }
                            }
                        }
                    }

                protected:
                };
            }
        }
    }