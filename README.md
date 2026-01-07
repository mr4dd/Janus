# Janus: A Dual Vector Approach to File Scoring

Janus is an experimental framework for categorizing files on a filesystem using two independent axes: Utility (U) and Sensitivity (Σ).

Standard filesystems treat data as a flat collection of attributes like size and timestamp. This project attempts to overlay a layer of "digital discernment" by mapping files to a 2D coordinate system. This determines a file's value to its creator versus its potential value to an adversary.

## The Theory of (U, Σ)

The project operates on the premise that file value is not a single number, but a relationship between two distinct vectors.

**The Utility Vector (U)**: Utility represents the "human signal" within the filesystem. It seeks to identify files that are the product of active work rather than machine generated noise. This score is derived by analyzing the entropy of the filename to detect human readable patterns, applying exponential decay to the file's age to measure current relevance, and weighting the file's depth within the directory hierarchy.

**The Sensitivity Vector (Σ)**: Sensitivity measures "Exploitation Potential." This axis focuses on the risk associated with a file's content, specifically seeking out PII, credentials, or system configurations. It utilizes a logarithmic scale to ensure that the discovery of a small amount of highly sensitive data triggers a significant risk score, acknowledging that data toxicity is often a binary state rather than a linear one.

## Project Status and Goals

This repository serves primarily as a proof-of-concept for the scoring theory. As an active work-in-progress, the core architectural logic and specific component scoring formulas are the current focus. Consequently, various parts of the implementation, such as the crawling engine or specific regex patterns; may be in a state of flux or not yet fully realized.

The primary objective is to refine the underlying math to better categorize data into actionable quadrants.

|                                |                                              |
|--------------------------------|----------------------------------------------|
|High Utility / Low Sensitivity  | Essential human assets.                      |
|High Utility / High Sensitivity | Critical business data requiring encryption. |
|Low Utility / High Sensitivity  | "Toxic waste" or legacy risk.                |
|Low Utility / Low Sensitivity   | System noise and temporary data.             |

Further development is required to calibrate the weighting constants and the decay functions. Feedback regarding the effectiveness of these mathematical models in distinguishing "signal from noise" on diverse datasets is the main priority of the current development phase.
