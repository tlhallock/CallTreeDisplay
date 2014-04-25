/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package guistuff;

import java.io.Serializable;
import java.util.List;

/**
 *
 * @author thallock
 */
public interface INode extends Serializable {
    INode getParent();
    List<INode> getChildren();
    String getName();
    int getSize();
    boolean exited();
    int getTotalTimesMethodCalled();
    boolean contains(String methodName);
}
